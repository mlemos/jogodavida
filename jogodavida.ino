/**************************************************************************
  * Jogo da Vida
  * Uma implementação do Jogo da Vida do Conway para Arduino.
  * An implementation of Conway's Game of Life for Arduino.
  * 
  * Por Manoel Lemos - @mlemos
  * ---> http://manoellemos.com
  * ---> http://fazedores.com/
  * 
  * MIT License
  * Copyright (c) 2016 Manoel Lemos
  *************************************************************************/

// Bibliotecas necessárias para o display OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Configurações do display OLED
// --> https://www.adafruit.com/product/326
#define OLED_RESET 6
Adafruit_SSD1306 display(OLED_RESET);

// Definições sobre o tamanho do universo que iremos simular
// Altere a escala para mudar o tamanho das células
#define ESCALA  4
#define LARGURA  128 / ESCALA
#define ALTURA  64 / ESCALA
#define TAMANHO_DO_VETOR_UNIVERSO LARGURA * ALTURA / 8

// Vetores que armazenarão os estados de todas as células
// Estamos usando um vetor de bytes (chars) para armazenar as células.
// Cada bit armazena o estado de uma célula (fizemos isto para economizar memória).
char universo[TAMANHO_DO_VETOR_UNIVERSO];
char novoUniverso[TAMANHO_DO_VETOR_UNIVERSO];

// Bloco de Setup
void setup()   {
  Serial.begin(9600);
  Serial.println("Jogo da Vida iniciado!");
  Serial.print("Universo com dimensões ");
  Serial.print(LARGURA);
  Serial.print(" x ");
  Serial.println(ALTURA);
  
  // Inicializa o display OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display.clearDisplay();
  display.display();
  
  // Apresenta a tela inicial
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(30,20);
  display.println("Jogo da Vida");
  display.setTextSize(1);
  display.setCursor(30,40);
  display.println("por @mlemos");  
  display.display();
  delay(2000);

  // Popula o universo com células em posições aleatórias
  // Altere o segundo parâmetro (densidade) para ter
  // mais ou menos cêlulas em seu universo.
  populaUniverso(universo,55);

  // Apresenta o universo no display OLED
  mostraUniverso(universo);
  delay(500);
}

// Laço principal
void loop() {
  // Evolui o universo 
  evoluiUniverso();
  // Apresenta o universo evoluido no display OLED
  mostraUniverso(universo);
  delay(500);
}

// Escreve um valor (1 ou 0) em uma cêlula X,Y do universo
// O primeiro parâmetro indica o vetor universo a ser usado.
void escreveCelula(char *ptr, int x, int y, int valor ){
  int cellPosition = y * LARGURA + x;
  int bytePosition = cellPosition / 8;
  int bitPosition = cellPosition % 8;
  char currentByte = ptr[bytePosition];
  bitWrite(currentByte,bitPosition,valor);
  ptr[bytePosition] = currentByte;
}

// Lê o valor (1 ou 0) de uma cêlula X,Y do universo
// O primeiro parâmetro indica o vetor universo a ser usado.
int leCelula(char *ptr, int x, int y){
  int cellPosition = y * LARGURA + x;
  int bytePosition = cellPosition / 8;
  int bitPosition = cellPosition % 8;
  char currentByte = ptr[bytePosition];
  int cellValue = bitRead(currentByte,bitPosition);
  return cellValue;
}

// Limpa (seta todas as cêlulas para 0) o universo
// O parâmetro indica o vetor universo a ser usado.
void limpaUniverso(char *ptr) {
  for (int i = 0; i < TAMANHO_DO_VETOR_UNIVERSO; ++i)
  {
    ptr[i] = 0;
  }
}

// Popula o universo com cêlulas vivas em posições aleatórias
// O primeiro parâmetro indica o vetor universo a ser usado.
// O segund determina a densidade de cêlulas (0-100).
void populaUniverso(char *ptr, int densidade) {
  limpaUniverso(ptr);
  int valorDaCelula;
  for (int x = 0; x < LARGURA; ++x)
  {
    for (int y = 0; y < ALTURA; ++y)
    {
      if (random(100) < (100 - densidade)) { 
        valorDaCelula = 0; // Celula Morta
      } else {
        valorDaCelula = 1; // Celula Viva
      }
      escreveCelula(ptr,x,y,valorDaCelula);
    }
  }
}

// Mostra o universo passado como parâmetro na serial
// Se você não possui um display OLED, pode modificar o programa
// para mostrar tudo na serial.
void mostraUniversoNaSerial(char *ptr) {
  Serial.println();
  for (int y = 0; y < ALTURA; ++y)
  {
    for (int x = 0; x < LARGURA; ++x)
    {
      Serial.print(leCelula(ptr,x,y));
    }
    Serial.println();
  }
  Serial.println();
}

// Mostra o universo passado como parâmetro no display OLED
void mostraUniverso(char *ptr){
  for (int y = 0; y < ALTURA; ++y)
  {
    for (int x = 0; x < LARGURA; ++x)
    {
      int color = leCelula(ptr,x,y);
      display.fillRect(x * ESCALA,y * ESCALA,ESCALA,ESCALA,color);
    }
  }
  display.display();  
}

// Conta o números de vizinhos (cêlulas vivas) de uma cêlula
// O primeiro parâmetro indica o vetor universo a ser usado.
int contaVizinhos(char *ptr, int x, int y) {
  int vizinhos = 0;
  int esquerda, direita, cima, baixo;
  // Calcula os valores x e y das cêlulas vizinhas.
  // Estamos usando uma implementação onde a parte de cima do universo
  // é conectada com a de baixo. E a esquerda é conectada com a direita.
  // Então aqui vemos se temos que lidar com isto ou não.
  if (x == 0) {
    esquerda = LARGURA - 1;
    direita = x + 1;
  } else if (x == (LARGURA - 1) ) {
    esquerda = x - 1;
    direita = 0;    
  } else {
    esquerda = x - 1;
    direita = x + 1;
  }
  if (y == 0) {
    cima = ALTURA - 1;
    baixo = y + 1;
  } else if (y == (ALTURA - 1) ) {
    cima = y - 1;
    baixo = 0;    
  } else {
    cima = y - 1;
    baixo = y + 1;
  }
  // Lê o valor de cada uma das 8 cêlulas vizinhas e soma para
  // encontrar o número de vizinhos da cêlula em questão.
  vizinhos = vizinhos + leCelula(ptr,esquerda ,cima   );  
  vizinhos = vizinhos + leCelula(ptr,x    ,cima   );  
  vizinhos = vizinhos + leCelula(ptr,direita,cima   );  
  vizinhos = vizinhos + leCelula(ptr,esquerda ,y     );  
  vizinhos = vizinhos + leCelula(ptr,direita,y     );  
  vizinhos = vizinhos + leCelula(ptr,esquerda ,baixo);  
  vizinhos = vizinhos + leCelula(ptr,x    ,baixo);  
  vizinhos = vizinhos + leCelula(ptr,direita,baixo);
  return vizinhos; 
}

// Calcula o novo estado de um cêlula aplicando as regras do Jogo da Vida
// O primeiro parâmetro indica o vetor universo a ser usado.
int novoEstado(char *ptr, int x, int y){
  int estadoAtual = leCelula(ptr,x,y);
  int novoEstado;
  int vizinhos = contaVizinhos(ptr,x,y);
  // Aplica as regras do Jogo da Vida
  if (estadoAtual == 1)
  { // Cêlula está viva
    if (vizinhos < 2) {
      novoEstado = 0; // Cêlula morre por solidão
    } else if (vizinhos > 3) {
      novoEstado = 0; // Cêlula morre por superpopulação
    } else {
      novoEstado = 1; // Cêlula continua viva (população ideal)
    }
  } else { // Cêlula está morta
    if (vizinhos == 3) {
      novoEstado = 1; // Cêlula torna-se viva (reprodução)
    } else {
      novoEstado = 0; // Cêlula continua morta
    }
  }
  return novoEstado;
}

// Evoluir o universo para a próxima geração
// Calcula os novos estados de todas as cêlulas do universo aplicando
// as regras do Jogo da Vida para cada uma delas.
void evoluiUniverso(){
  limpaUniverso(novoUniverso);
  for (int y = 0; y < ALTURA; ++y)
  {
    for (int x = 0; x < LARGURA; ++x)
    {
      escreveCelula(novoUniverso,x,y,novoEstado(universo,x,y));
    }
  }
  for (int i = 0; i < TAMANHO_DO_VETOR_UNIVERSO; ++i)
    {
      universo[i] = novoUniverso[i];
    }  
}
