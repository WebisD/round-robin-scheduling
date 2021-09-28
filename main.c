// Comment this line if you don't have opengl installed
#define _opengl

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
    int *start_time;
    int length_start_time;
    int remaining_time;
}Process;

int quantum = INT_MAX;
int quant_process;
Process* all_process;
int total_duration = 0;

int* splits_cpu;
int total_splits = 0;

#define MAX 10000

Process* queue[MAX];
int front = 0;
int rear = -1;
int itemCount = 0;

/*################################## Function Declarations ################################*/

int compare_arrival(const void *a, const void *b);
int compare_pid(const void *a, const void *b);
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
void sort_process(){

    for (int i = 0; i < quant_process; i++)
    {
        total_splits += all_process[i].length_start_time;
    }

    //printf("%d", total_splits);

    int* splits = malloc(sizeof(int) * total_splits);;
    
    int greater = INT_MAX;

    for (int k = 0; k < total_splits; k++)
    {    
        for (int i = 0; i < quant_process; i++) {
            for (int j = 0; j < all_process[i].length_start_time; j++)
            {
                int z = all_process[i].start_time[j];
                if (all_process[i].start_time[j] < greater){
                    if (k>0 && all_process[i].start_time[j] > splits[k-1]){
                        greater = all_process[i].start_time[j];
                    }
                    else if(k==0){
                        greater = all_process[i].start_time[j];
                    }
                }
            }
        }
        splits[k] = greater;
        greater = INT_MAX;
    }

    splits_cpu = malloc(sizeof(int) * total_splits);
    splits_cpu = splits;

    return;
}
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
    float cpu_unity = (cpu_P3.x - cpu_P1.x)/total_duration;
    int j = 0;
    float i = 0;

    char num[10] = "0";
    output(cpu_P2.x-5, cpu_P2.y-20, num);  

    printf("oi %d \n", total_splits);

    for (int i = 1; i < total_splits; i++)
    {
        //printf("%d - %f", splits_cpu[i], (cpu_P1.x + cpu_unity)*splits_cpu[i]);

        double x = cpu_P1.x + cpu_unity*splits_cpu[i];

        glBegin(GL_LINE_STRIP);
            glVertex2i(x,cpu_P1.y);
            glVertex2i(x,cpu_P2.y);
        glEnd();

        sprintf(num, "%d", splits_cpu[i]);
        if (j >= 10){
            output(x-10, cpu_P2.y-20, num);
        }
        else{
            output(x-5, cpu_P2.y-20, num);
        }
    }
    
    
    /*for (float i = cpu_P1.x+cpu_unity; i < cpu_P3.x; i = i + cpu_unity)
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
    }**/

    /*j++;
    sprintf(num, "%d", j); 
    output(cpu_P3.x-5, cpu_P2.y-20, num);*/
    
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


int compare_arrival(const void *a, const void *b) {
    return ( (*(Process*)a).arrival - (*(Process*)b).arrival);
}

int compare_pid(const void *a, const void *b) {
    return ( (*(Process*)a).pid - (*(Process*)b).pid);
}

float round_robin() {
    qsort(all_process, quant_process, sizeof(Process), compare_arrival);

    int i, j;
    float average_waiting_time = 0;

    insert(&all_process[0]);
    peek()->start_time[0] = 0;
    peek()->length_start_time ++;

    int running_processes = quant_process;

    Process *current_process;
    for (i = 0; i <= total_duration; i++) {
        current_process = peek();

        // printf("\n");
        // print_queue();
        // printf("i: %d ----------- Processo P%d -----------\n", i, current_process->pid);

        if (i == 0) {
            printf("Time -%2d - start\n", i);
        }

        if (i != 0){
            current_process->remaining_time -= 1;
        }
        
        //Process has finished
        if (current_process->remaining_time == 0) {
            // printf("\tAcabou processo p%d\n", current_process->pid);
            
            // print_process(current_process->pid);
            // printf("\ttime_taken = %d - %d - %d\n", i, current_process->duration, current_process->arrival);
            current_process->time_taken = i - current_process->duration - current_process->arrival;

            // print_process(current_process->pid);
            average_waiting_time += i - current_process->duration - current_process->arrival;
            
            Process *processo = removeData();
            // printf("\tRemoveu processo P%d do ínicio da fila\n", processo->pid);
            printf("Time -%2d - end of process P%d\n", i, current_process->pid);

            if (isEmpty() == 0){
                peek()->start_time[peek()->length_start_time] = i;
                peek()->length_start_time++;
            }

            running_processes -= 1;
            if (running_processes == 0){
                print_queue();
                break;
            }
        }

        for (j = 0; j < current_process->number_of_IO; j++) {
            // I/O happened
            if (current_process->duration - current_process->remaining_time == current_process->IO[j]) {
                // printf("\tformula: %d - %d = %d\n", current_process->duration, current_process->remaining_time, current_process->IO[j]);

                // printf("\tIO do processo p%d aconteceu!\n", current_process->pid);

                printf("Time -%2d - I/O operation: P%d\n", i, current_process->pid);

                Process *processo = removeData();
                // printf("\tRemoveu processo P%d do ínicio da fila\n", processo->pid);

                insert(processo);

                // printf("\tInseriu processo P%d no final da fila\n", processo->pid);
                peek()->start_time[peek()->length_start_time] = i;
                peek()->length_start_time++;

                // printf("\tTrocando do processo P%d para o processo P%d\n", current_process->pid, peek()->pid);
            }
        }

        // Quantum happened
        if (i != 0 && current_process->start_time[peek()->length_start_time-1] + quantum == i){
            // printf("\tQuantum ocorreu!\n");
            
            Process *processo = removeData();
            // printf("\tRemoveu processo P%d do ínicio da fila\n", processo->pid);

            insert(processo);

            peek()->start_time[peek()->length_start_time] = i;
            peek()->length_start_time++;
            // printf("\tInseriu processo P%d no final da fila\n", processo->pid);
            
            // printf("\tTrocando do processo P%d para o processo P%d\n", current_process->pid, peek()->pid);

            printf("Time -%2d - end of quantum: P%d\n", i, current_process->pid);
        }

        for (j = 1; j < quant_process; j++) { 
            if (all_process[j].arrival == i) {
                printf("Time -%2d - arrival of process: P%d\n", i, all_process[j].pid);
                insert(&all_process[j]);
            }
        }

        print_queue();
        printf("On CPU: P%d\n", peek()->pid);
        
        printf("\n");
    }

    // printf("Acabou round robin!\n");
    printf("END!\n\n");

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

    if (isEmpty() == 1) {
        printf(" empty\n");
        return;
    }

    for (int i = front; i <= rear; i++) {
        printf(" P%d(%d)", queue[i]->pid, queue[i]->remaining_time);   
        
        if (i < rear){
            printf(",");
        }
    }

    printf("\n");
}


/*##################################   Main   ##################################*/


void get_lazy_inputs(){
    quantum = 4;
    quant_process = 5;

    all_process = malloc(sizeof(Process) * quant_process);
    int quant_IO[] = {4,1,1,2,0}, _i_o;

    int dur[] = {9,10,5,7,2};
    int arr[] = {10,4,0,1,17};

    for (int i = 0; i < quant_process; i++)
    {
        Process temp;
        temp.time_taken = 0;

        temp.pid = i+1;

        temp.duration = dur[i];

        total_duration += temp.duration;
        temp.remaining_time = temp.duration;

        temp.arrival = arr[i];

        temp.IO = malloc(sizeof(int) * quant_IO[i]);

        int *all_IO = malloc(sizeof(int) * quant_IO[i]);
        temp.number_of_IO = quant_IO[i];

        if (i == 0){
            int a[] = {2,4,6,8};
            for (int j = 0; j < quant_IO[i]; j++){
                all_IO[j] = a[j]; 
            }
            
        }
        if (i == 1){
            int a[] = {5};
            for (int j = 0; j < quant_IO[i]; j++){
                all_IO[j] = a[j]; 
            }
            
        }
        if (i == 2){
            int a[] = {2};
            for (int j = 0; j < quant_IO[i]; j++){
                all_IO[j] = a[j]; 
            }
            
        }
        if (i == 3){
            int a[] = {3,6};
            for (int j = 0; j < quant_IO[i]; j++){
                all_IO[j] = a[j]; 
            }
            
        }
        if (i == 4){
            int a[] = {};
            for (int j = 0; j < quant_IO[i]; j++){
                all_IO[j] = a[j]; 
            }
            
        }

        temp.IO = all_IO;
        all_process[i] = temp;

    }

    for (int i = 0; i < quant_process; i++)
    {
        all_process[i].start_time = malloc(sizeof(int) * total_duration);
        all_process[i].length_start_time = 0;
    }
    
    return;
}

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
        temp.remaining_time = temp.duration;
        
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

    for (int i = 0; i < quant_process; i++)
    {
        all_process[i].start_time = malloc(sizeof(int) * total_duration);
        all_process[i].length_start_time = 0;
    }
    

    return;
}   

// Programa Principal
int main(int argc, char **argv)
{
    get_lazy_inputs();

    printf("\n\n");

    float average_waiting_time = round_robin();

    qsort(all_process, quant_process, sizeof(Process), compare_pid);
    // print_list();
    printf("Waiting times:\n");

    for(int i = 0; i < quant_process; i++) { 
        printf("P%d: %3dms\n", all_process[i].pid, all_process[i].time_taken);
    }

    printf("\nAverage waiting time: %.2fms\n", average_waiting_time);

    for (int i = 0; i < quant_process; i++) {
        printf("%d", all_process[i].pid);
        for (int j = 0; j < all_process[i].length_start_time; j++)
        {
            printf(" - %d", all_process[i].start_time[j]);
        }
        printf("\n");
    }


    sort_process();

    printf("\n");

    for (int i = 0; i < total_splits; i++)
    {
        printf("%d ", splits_cpu[i]);
    }
    printf("\n");

    #ifdef _opengl
        mainOpengl(argc, argv);
    #endif

    return 0;
}

