// Comment this line if you don't have opengl installed
//#define _opengl

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef _opengl
#include <GL/freeglut.h>
#include <GL/glut.h>
#endif


typedef struct 
{
	float x;
	float y;
}Ponto;

typedef struct 
{
    int pid;
	int duration;
    int arrival;
    int *i_o;
}Process;

int Quantum = INT_MAX;
int quant_process;
Process* all_process;

#define MAX 100

int intArray[MAX];
int front = 0;
int rear = -1;
int itemCount = 0;

/*################################## Function Declarations ################################*/

int compare_process(const void *a, const void *b);
float round_robin();
void print_list();
void get_inputs();

/*##################################   Opengl   ##################################*/

Ponto cpu_P1 = {40, 150};
Ponto cpu_P2 = {40, 100};
Ponto cpu_P3 = {600, 100};
Ponto cpu_P4 = {600, 150};

// Função callback chamada para gerenciar eventos de teclas
#ifdef _opengl

void Teclado(unsigned char key, int x, int y)
{
	if (key == 27)  // sai comm ESC
		exit(0);
}

void output(GLfloat x, GLfloat y, char *text)
{
    char *p;
    
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(0.1, 0.1, 0.1);
    glLineWidth(1.2);
    for (p = text; *p; p++)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
    glPopMatrix();
}

void split_cpu(){
    float cpu_unity = (cpu_P3.x - cpu_P1.x)/33;
    int j = 0;
    float i = 0;
    char num[10] = "0";

    output(cpu_P2.x-5, cpu_P2.y-20, num);  
    
    for (float i = cpu_P1.x+cpu_unity; i < cpu_P3.x; i = i + cpu_unity)
    {
        glBegin(GL_LINE_STRIP);
            glVertex2i(i,cpu_P1.y);
            glVertex2i(i,cpu_P2.y);
        glEnd();

        j++;
        sprintf(num, "%d", j);
        if (j >= 10){
            output(i-10, cpu_P2.y-20, num);
        }
        else{
            output(i-5, cpu_P2.y-20, num);
        }
    }

    j++;
    sprintf(num, "%d", j); 
    output(cpu_P3.x-5, cpu_P2.y-20, num);
    
}

// Função callback chamada para fazer o desenho
void Desenha(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

     // Limpa a janela de visualização com a cor de fundo especificada
    glClear(GL_COLOR_BUFFER_BIT);

     // Especifica que a cor corrente é vermelha
     //         R     G     B
    glColor3f(1.0f, 1.0f, 1.0f);

     // Desenha um quadrado preenchido com a cor corrente
    glBegin(GL_LINE_LOOP);
        glVertex2i(cpu_P1.x,cpu_P1.y);
        glVertex2i(cpu_P2.x,cpu_P2.y);
        glVertex2i(cpu_P3.x,cpu_P3.y);
        glVertex2i(cpu_P4.x,cpu_P4.y);
    glEnd();

    split_cpu();
    glFlush();
}

// Inicializa parâmetros de rendering
void Inicializa (void)
{
    // Define a cor de fundo da janela de visualização como preta
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// Função callback chamada quando o tamanho da janela é alterado
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
    // Evita a divisao por zero
    if(h == 0) h = 1;

    // Especifica as dimensões da Viewport
    glViewport(0, 0, w, h);

    // Inicializa o sistema de coordenadas
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Estabelece a janela de seleção (left, right, bottom, top)
    if (w <= h)
            gluOrtho2D (0.0f, 250.0f, 0.0f, 250.0f*h/w);
    else
            gluOrtho2D (0.0f, 250.0f*w/h, 0.0f, 250.0f);
}

void mainOpengl(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(900,350);
    glutInitWindowPosition(10,10);
    glutCreateWindow("Round Robin Scheduling");
    glutDisplayFunc(Desenha);
    glutReshapeFunc(AlteraTamanhoJanela);
    
    // Registra a função callback para tratamento das teclas ASCII
	glutKeyboardFunc(Teclado);

    Inicializa();
    glutMainLoop();
    return;
}
#endif


/*##################################   Queue   ##################################*/

int peek() {
   return intArray[front];
}

int isEmpty() {
   return itemCount == 0;
}

int isFull() {
   return itemCount == MAX;
}

int size() {
   return itemCount;
}  

void insert(int data) {

   if(!isFull()) {
	
      if(rear == MAX-1) {
         rear = -1;            
      }       

      intArray[++rear] = data;
      itemCount++;
   }
}

int removeData() {
   int data = intArray[front++];
	
   if(front == MAX) {
      front = 0;
   }
	
   itemCount--;
   return data;  
}

/*##################################   RR Scheduling   ##################################*/

int compare_process(const void *a, const void *b) {
    return ( (*(Process*)a).arrival - (*(Process*)b).arrival);
}

float round_robin() {
    qsort(all_process, quant_process, sizeof(Process), compare_process);
    print_list();
}

void print_list() {
    printf("\nList of Processes\n");

    for (int i = 0; i < quant_process; i++) {
        printf("Process P%d\n", all_process[i].pid);
        printf("\tduration = %dms\n", all_process[i].duration);
        printf("\tarrival = %dms\n", all_process[i].arrival);
    }

    printf("\n");
}

/*##################################   Main   ##################################*/
void get_inputs(){
    printf("Round Robin Scheduling\n");
    
    printf("What is the value of the quantum: ");
    scanf("%d", &Quantum);
    
    printf("How many processes do you have: ");
    scanf("%d", &quant_process);

    all_process = malloc(sizeof(Process) * quant_process);
    Process temp;
    int quant_i_o, _i_o;

    for (int i = 0; i < quant_process; i++)
    {

        printf("--- Info of Process P%d ---\n", i+1);
        temp.pid = i+1;
        
        printf("Duration in ms: ");
        scanf("%d", &temp.duration);
        
        printf("Arrival time in ms: ");
        scanf("%d", &temp.arrival);
        
        printf("How many I/O does the process have: ");
        scanf("%d", &quant_i_o);
        
        int *all_i_o;
        all_i_o = malloc(sizeof(int) * quant_i_o);

        printf("Put the I/O in ascending order: \n");
        for (int j = 0; j < quant_i_o; j++)
        {
            printf("I/0: ");
            scanf("%d", &_i_o);
            all_i_o[j] = _i_o;
        }

        temp.i_o = all_i_o;
        all_process[i] = temp;
    }
    
    print_list();


    return;
}   

// Programa Principal
int main(int argc, char **argv)
{
    #ifdef _opengl
        mainOpengl(argc, argv);
    #endif

    get_inputs();

    float time_taken = round_robin();

    return 0;
}

