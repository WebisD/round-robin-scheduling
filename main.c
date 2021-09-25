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
    int number_of_IO;
    int *IO;
    int time_taken;
}Process;

int quantum = INT_MAX;
int quant_process;
Process* all_process;
int total_duration = 0;

#define MAX 10000

Process* queue[MAX];
int front = 0;
int rear = -1;
int itemCount = 0;

/*################################## Function Declarations ################################*/

int compare_process(const void *a, const void *b);
float round_robin();
void print_list();
void get_inputs();
void print_process(int pid);
void print_queue();

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

Process* peek() {
    return queue[front];
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

void insert(Process* data) {

    if(!isFull()) {

        if(rear == MAX-1) {
            rear = -1;            
        }       

        rear++;

        queue[rear] = data;
        itemCount++;
    }
}

Process* removeData() {
    Process *data = queue[front];

    front++;

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

    int i, j;
    int average_waiting_time = 0;

    insert(&all_process[0]);

    Process *current_process;
    for (i = 0; i <= total_duration; i++) {
        current_process = peek();

        printf("\n");
        print_queue();
        printf("i: %d ----------- Processo P%d -----------\n", i, current_process->pid);

        current_process->duration -= 1;
        
        if (current_process->duration == 0) {
            printf("\tAcabou processo p%d\n", current_process->pid);
            print_process(current_process->pid);
            current_process->time_taken = i;
            print_process(current_process->pid);
            average_waiting_time += i;
            
            removeData();
        }

        for (j = 0; j < current_process->number_of_IO; j++) {
            if (current_process->IO[j] == i) {
                printf("\tIO do processo p%d aconteceu!\n", current_process->pid);
                insert(removeData());

                printf("\tTrocando do processo P%d para o processo P%d\n", current_process->pid, peek()->pid);
            }
        }

        if (i != 0 && i%quantum == 0){
            printf("\tQuantum ocorreu!\n");
            Process *processo = removeData();
            insert(processo);
            // printf("\tTrocando do processo P%d para o processo P%d\n", current_process->pid, peek()->pid);
        }

        for (j = 1; j < quant_process; j++) { 
            if (all_process[j].arrival == i) {
                printf("\tChegada do process P%d\n", all_process[j].pid);
                insert(&all_process[j]);
            }
        }
    }

    printf("Acabou round robin!\n");

    return average_waiting_time / quant_process;
}

void print_list() {
    printf("\nList of Processes\n");

    for (int i = 0; i < quant_process; i++) {
        printf("Process P%d\n", all_process[i].pid);
        printf("\tduration = %dms\n", all_process[i].duration);
        printf("\tarrival = %dms\n", all_process[i].arrival);
        printf("\ttime_taken = %dms\n", all_process[i].time_taken);
    }

    printf("\n");
}

void print_process(int pid) {
    printf("\n\tProcess P%d\n", pid);

    for (int i = 0; i < quant_process; i++) {
        if (all_process[i].pid == pid) {
            printf("\t\tduration = %dms\n", all_process[i].duration);
            printf("\t\tarrival = %dms\n", all_process[i].arrival);
            printf("\t\ttime_taken = %dms\n", all_process[i].time_taken);
            printf("\t\tnumber_of_IO = %dms\n", all_process[i].number_of_IO);
        }
    }

    printf("\n");
}

void print_queue() {
    printf("Queue:");

    for (int i = front; i < itemCount; i++) {
        printf(" %d", queue[i]->pid);   
    }

    printf("\n");
}

/*##################################   Main   ##################################*/
void get_inputs(){
    printf("Round Robin Scheduling\n");
    
    printf("What is the value of the quantum: ");
    scanf("%d", &quantum);
    
    printf("How many processes do you have: ");
    scanf("%d", &quant_process);

    all_process = malloc(sizeof(Process) * quant_process);
    int quant_IO, _i_o;

    for (int i = 0; i < quant_process; i++)
    {
        Process temp;
        temp.time_taken = 0;

        printf("--- Info of Process P%d ---\n", i+1);
        temp.pid = i+1;
        
        printf("Duration in ms: ");
        scanf("%d", &temp.duration);

        total_duration += temp.duration;
        
        printf("Arrival time in ms: ");
        scanf("%d", &temp.arrival);
        
        printf("How many I/O does the process have: ");
        scanf("%d", &quant_IO);
        
        temp.IO = malloc(sizeof(int) * quant_IO);

        int *all_IO = malloc(sizeof(int) * quant_IO);
        temp.number_of_IO = quant_IO;

        printf("Put the I/O in ascending order: \n");
        for (int j = 0; j < quant_IO; j++)
        {
            printf("I/0: ");
            scanf("%d", &_i_o);
            all_IO[j] = _i_o;
        }

        temp.IO = all_IO;
        all_process[i] = temp;
    }

    return;
}   

// Programa Principal
int main(int argc, char **argv)
{
    #ifdef _opengl
        mainOpengl(argc, argv);
    #endif

    get_inputs();

    printf("\n\n");

    float average_waiting_time = round_robin();

    print_list();
    printf("Tempo de Espera Médio: %.2f", average_waiting_time);

    return 0;
}

