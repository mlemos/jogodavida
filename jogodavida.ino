/***************************************************
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
  ****************************************************/

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
  drawGrid(universo);
  delay(500);

}

// Laço principal
void loop() {

  // Evolui o universo 
  evolveUniverse();

  // Apresenta o universo evoluido no display OLED
  drawGrid(universo);
  delay(500);
}

void writeCell(char *ptr, int x, int y, int value ){
  int cellPosition = y * LARGURA + x;
  int bytePosition = cellPosition / 8;
  int bitPosition = cellPosition % 8;
  char currentByte = ptr[bytePosition];
  bitWrite(currentByte,bitPosition,value);
  ptr[bytePosition] = currentByte;
}

int readCell(char *ptr, int x, int y){
  int cellPosition = y * LARGURA + x;
  int bytePosition = cellPosition / 8;
  int bitPosition = cellPosition % 8;
  char currentByte = ptr[bytePosition];
  int cellValue = bitRead(currentByte,bitPosition);
  return cellValue;
}

void clearUniverse(char *ptr) {
  for (int i = 0; i < TAMANHO_DO_VETOR_UNIVERSO; ++i)
  {
    ptr[i] = 0;
  }
}

void populateTestUniverse(char *ptr) {
  int color = 0;
  clearUniverse(ptr);
  for (int y = 0; y < ALTURA; ++y)
  { 
    color = (color == 0) ? 1 : 0;
    for (int x = 0; x < LARGURA; ++x)
    {
      color = (color == 0) ? 1 : 0;
      writeCell(ptr,x,y,color);
    }
  }
}

void populaUniverso(char *ptr, int density) {
  Serial.print("Populating universo with density = ");
  Serial.print(density);
  Serial.print(" ... ");

  clearUniverse(ptr);
  int population = 0;
  int threshold = 100 - density;
  int cellStatus;
  for (int x = 0; x < LARGURA; ++x)
  {
    for (int y = 0; y < ALTURA; ++y)
    {
      if (random(100) < threshold) { 
        cellStatus = 0;
      } else {
        cellStatus = 1;
      }
      //cellStatus = 0;
      writeCell(ptr,x,y,cellStatus);
      population = population + cellStatus;
    }
  }
  //writeCell(ptr,4,4,0);
  //writeCell(ptr,5,4,1);
  //writeCell(ptr,6,4,0);
  //writeCell(ptr,4,5,0);
  //writeCell(ptr,5,5,0);
  //writeCell(ptr,6,5,1);
  //writeCell(ptr,4,6,1);
  //writeCell(ptr,5,6,1);
  //writeCell(ptr,6,6,1);
  Serial.print("Population = ");
  Serial.println(population);
}

void serialDrawGrid(char *ptr) {
  Serial.println("====================================");
  for (int y = 0; y < ALTURA; ++y)
  {
    for (int x = 0; x < LARGURA; ++x)
    {
      Serial.print(readCell(ptr,x,y));
    }
    Serial.println();
  }
  Serial.println("====================================");
}

void drawGrid(char *ptr){
  for (int y = 0; y < ALTURA; ++y)
  {
    for (int x = 0; x < LARGURA; ++x)
    {
      int color = readCell(ptr,x,y);
      display.fillRect(x * ESCALA,y * ESCALA,ESCALA,ESCALA,color);
    }
  }
  display.display();  
}

int countNeighbours(char *ptr, int x, int y) {
  int neighbours = 0;
  int left, right, top, bottom;
  if (x == 0) {
    left = LARGURA - 1;
    right = x + 1;
  } else if (x == (LARGURA - 1) ) {
    left = x - 1;
    right = 0;    
  } else {
    left = x - 1;
    right = x + 1;
  }
  if (y == 0) {
    top = ALTURA - 1;
    bottom = y + 1;
  } else if (y == (ALTURA - 1) ) {
    top = y - 1;
    bottom = 0;    
  } else {
    top = y - 1;
    bottom = y + 1;
  }
  neighbours = neighbours + readCell(ptr,left ,top   );  
  neighbours = neighbours + readCell(ptr,x    ,top   );  
  neighbours = neighbours + readCell(ptr,right,top   );  
  neighbours = neighbours + readCell(ptr,left ,y     );  
  neighbours = neighbours + readCell(ptr,right,y     );  
  neighbours = neighbours + readCell(ptr,left ,bottom);  
  neighbours = neighbours + readCell(ptr,x    ,bottom);  
  neighbours = neighbours + readCell(ptr,right,bottom);
  return neighbours; 
}

int newState(int currentState, int neighbours){
  if (currentState == 1)
  { // Cell is Alive
    if (neighbours < 2) {
      return 0;
    } else if ((neighbours == 2) || (neighbours == 3)) {
      return 1;
    } else {
      return 0;
    }
  } else { // Cell is Dead
    if (neighbours == 3) {
      return 1;
    } else {
      return 0;
    }
  }
}

void evolveUniverse(){
  clearUniverse(novoUniverso);
  for (int y = 0; y < ALTURA; ++y)
  {
    for (int x = 0; x < LARGURA; ++x)
    {
      int currentState = readCell(universo,x,y);
      int neighbours = countNeighbours(universo,x,y);
      //Serial.print(neighbours);
      writeCell(novoUniverso,x,y,newState(currentState,neighbours));
    }
    //Serial.println();
  }
  for (int i = 0; i < TAMANHO_DO_VETOR_UNIVERSO; ++i)
    {
      universo[i] = novoUniverso[i];
    }  
}






