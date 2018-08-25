#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <ctype.h>
#include <termios.h>
#include <unistd.h>

#define clear_all() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))

#define SCREEN_WIDTH 81

#define SCREEN_TOP 3
#define SCREEN_WORK_HEIGHT 22

#define WIDTH_INFO_STR 20


/*
    функция для отображения подсказок пользователю
*/
void create_canvas(int x, int y){
    gotoxy(0, SCREEN_WIDTH - 14);
    printf("|\033[1m MiniEd v.01 \033[0m|");

    gotoxy(SCREEN_TOP-1, 0);
    for(int i = 0; i < SCREEN_WIDTH-1; i++){
        putchar('-');
    }

    gotoxy(SCREEN_WORK_HEIGHT+1, 0);
    for(int i = 0; i < SCREEN_WIDTH-1; i++){
        putchar('-');
    }

    gotoxy(SCREEN_WORK_HEIGHT+2, 20);
    printf("|\033[1m ^C - Exit without saving \033[0m|");

    gotoxy(SCREEN_WORK_HEIGHT+2, 47);
    printf("|\033[1m ^O - Save \033[0m|");

    gotoxy(x, y);
}

/*
    создание и очистка временной директории
*/
void clear_temp_dir(){
    system("rm -rf ./temp/*");
}

void create_temp_dir(){
    system("mkdir -p temp");
    clear_temp_dir();
}

/*
    очистка текущей строки
*/
void clear_current_str(char arr[]){
    for(int i = 0; i < SCREEN_WIDTH; i++){
        arr[i] = '\0';
    }
}


/*
    функция для очистки требуемой строки на экране
*/
void clear_string_on_display(int x){
    for(int i = 0; i < SCREEN_WIDTH; i++){
        gotoxy(x, i);
        putchar(' ');
    }
}


/*
    функция для вывода информации пользователю
*/
void show_info_message(char msg[], int x, int y){
    for(int i = 0; i < WIDTH_INFO_STR; i++){
        gotoxy(0, i);
        putchar(' ');
    }
    gotoxy(0,0);
    printf("%s", msg);

    gotoxy(x, y);
}

/*
    функция перезаписи символа на экране
*/
void rewrite_symbol(int x, int y, char ch){
    gotoxy(x, y+1);
    putchar(ch);
}

/*
    функция для проверки символа конца строки
*/
int check_null_symbol(char str[], int pos){
    for(int i = 0; i < SCREEN_WIDTH; i++){
        if(str[i] == '\0' && i > pos){
            return 1;
        }
    }
    return 0;

}

/*
    сохранение строки в файл
*/

void save_string(char arr[], int num_str, int x, int y){
    FILE *fp, *fp1, *fp2;
    int count_str = 0;
    char ch;
    int double_new_line = 0;
    if((fp1 = fopen("./temp/temp1.mini", "r"))!=NULL){
        while((ch = fgetc(fp1))!= EOF){
            if(ch == '\n' && double_new_line){
                double_new_line = 0;
                continue;
            }
            else if(ch == '\n'){
                double_new_line = 1;
                count_str += 1;
            }        
        }
        fclose(fp1);
    }        

    if((fp = fopen("./temp/temp1.mini", "a"))==NULL){
        show_info_message("Ошибка открытия файла temp1.", x, y);
        return;
    }


    //запись в файл
    if(count_str <= num_str || count_str == 0){
        for(int i = 0; i < SCREEN_WIDTH; i++){
            if(arr[i] != '\0'){
                putc(arr[i], fp);
            }
            else if(arr[i] == '\0' && i == 0){
                break;
            }
            else{
                break;
            }
            
        }
        putc('\n', fp);
    }
    else{
        //редактирование строки в файле.
        fclose(fp);
        //делаем копию файла
        if((fp2 = fopen("./temp/temp2.mini", "w"))==NULL){
            show_info_message("Ошибка открытия файла temp2.", x, y);
            return;
        }

        if((fp = fopen("./temp/temp1.mini", "r"))==NULL){
            show_info_message("Ошибка открытия файла temp1.", x, y);
            return;
        }
        
        while((ch = getc(fp)) != EOF){
            putc(ch, fp2);
        }

        fclose(fp2);
        fclose(fp);

        
        if((fp2 = fopen("./temp/temp2.mini", "r"))==NULL){
            show_info_message("Ошибка открытия файла temp2.", x, y);
            return;
        }

        if((fp = fopen("./temp/temp1.mini", "w"))==NULL){
            show_info_message("Ошибка открытия файла temp1.", x, y);
            return;
        }
        char str[SCREEN_WIDTH];
        count_str = 0;
        double_new_line = 0;
        int str_is_find = 0;


        while(!feof(fp2)){
            if(fgets(str, SCREEN_WIDTH, fp2)){
                if(count_str == num_str){
                    putc('\n', fp);
                    for(int i = 0; i < SCREEN_WIDTH-2; i++){
                        if(arr[i] != '\0'){
                            putc(arr[i], fp);
                        }
                        else{
                            
                            break;
                        }
                        
                        
                    }
                    
                    putc('\n', fp);
                    count_str++;
                    double_new_line = 0;
                    str_is_find = 1;
                    continue;
                }
                else{
                    if(str_is_find){
                        str_is_find = 0;
                        continue;
                    }
                    if(!double_new_line){
                        count_str++;
                        double_new_line = 1;
                    }
                    else{
                        double_new_line = 0;
                    }            
                    fputs(str, fp);
                }
            }
        }

    }

    fclose(fp);
}


int save_file(char *filename, int x, int y){

    FILE *origin, *temp;
    char ch;
    int double_new_line = 0;
    if((temp = fopen("./temp/temp1.mini", "r"))==NULL){
        show_info_message("Ошибка открытия файла", x, y);
        return -1;
    }

    if((origin = fopen(filename, "w"))==NULL){
        show_info_message("Ошибка открытия оригин. файла", x, y);
        return -1;
    }

    while((ch = fgetc(temp)) != EOF){
        if(ch == '\n' && double_new_line){
            double_new_line = 0;
            continue;
        }
        else if(ch == '\n'){
            double_new_line = 1;
        }
        putc(ch, origin);
    }
    return 0;
}

/*
    функция для чтения запрашиваемой строки из файла
*/

int read_current_str_from_file(char current_str[], int num, int x, int *y, char mode[]){
    FILE *fp;
    if((fp = fopen("./temp/temp1.mini", "r"))==NULL){
        show_info_message("Ошибка открытия файла", x, *y);
        return -1;
    }
    char ch;
    int count = 0;
    int flag = 0;
    int current_line = 0;
    int empty_str = 0;
    int double_new_line = 0;
    int is_clear_current_str = 0;
    if(mode == "up"){
        num -= 1;
        x--;
    }
    else{
        num += 1;
        x++;
    }



    while((ch = fgetc(fp))!= EOF){
        if(ch != '\n' && count < SCREEN_WIDTH){
            if(current_line == num){
                if(!is_clear_current_str){
                    clear_current_str(current_str);
                    is_clear_current_str = 1;
                }
                current_str[count] = ch;
                count++;
                flag = 1;
                
            }
            double_new_line = 0;
            
        }
        else if(ch == '\n' && (current_line == num) && !flag && !double_new_line){
            if(!is_clear_current_str){
                clear_current_str(current_str);
                is_clear_current_str = 1;
            }
            empty_str = 1;
            flag = 1;
            break;
            
        }
        else{
            if(double_new_line){
                double_new_line = 0;
                continue;
            }
            current_line += 1;
            double_new_line = 1;
        }
        if(current_line > num){
            break;
        }    

    }

    fclose(fp);

    if(empty_str){
        *y = 0;
        current_str[0] = '\n';
        current_str[1] = '\0';
        int i = 0;
        clear_string_on_display(x);
        while(current_str[i] != '\0'){
            rewrite_symbol(x, i, current_str[i]);
            i++;
        }
        return 1;
    }
    if(flag){
        *y = count+1;
        current_str[count] = '\0';
        int i = 0;
        clear_string_on_display(x);
        while(current_str[i] != '\0'){
            rewrite_symbol(x, i, current_str[i]);
            i++;
        }
        return 1;
    }
    else{
        if(current_line >= num){
            return -1;
        }
        *y = 0;
        return 0;
    }
    
}

/*
    функция для чтения и вывода на экран содержимого текстового файла
*/
void read_file_and_show(FILE *fp, char current_str[], int *x, int *y, int *num_str){
    char ch;
    int flag = 0;
    int y_pos;
    int count_symbols = 0;
    FILE *temp;

    /*записываем данные файла во временный файл
      каждый символ новой строки дублируем, чтобы при записи не путаться с теми
      символами \n, которые служат для переноса строки на экране, если длина её
      больше 80 символов
    */
    temp = fopen("./temp/temp1.mini", "w");
    while((ch = fgetc(fp)) != EOF){
        
        if(ch == '\n'){
            putc('\n', temp);
            putc('\n', temp);
            count_symbols = 0;
        }
        
        if(count_symbols > SCREEN_WIDTH){
            putc('\n', temp);
            count_symbols = 0;
        }
        if(ch > 31 && ch < 128){
            putc(ch, temp);
            count_symbols++;
        }

    }
    fclose(temp);

    //выводим на экран содержимое временного файла
    //переносы строк по ширине экрана SCREEN_WIDTH
    temp = fopen("./temp/temp1.mini", "r");

    int new_line = 0;

    while((ch = fgetc(temp))!= EOF && (*x < SCREEN_WORK_HEIGHT+1)){
        //новая строка 
        if(ch == '\n'){
            if(new_line){
                new_line = 0;
                continue;
            }
            new_line = 1;
            flag = 1;
            y_pos = *y;
            *y = 0;
            *x += 1;
            *num_str += 1;
            gotoxy(*x, *y);
            continue;
        }
        
        if(*y > SCREEN_WIDTH-1){
            *x += 1;
            *y = 0;
            
        }
        if(ch > 31 && ch < 128){
            if(new_line){
                new_line = 0;
            }
            *y += 1;
            gotoxy(*x, *y);
            putchar(ch);
            current_str[*y-1] = ch;
            
        }
    }
    if(flag){
        *x -= 1;
        *y = y_pos+1;
        gotoxy(*x, *y);
        if(*num_str > 0){
            *num_str -= 1;
        }
    }
    fclose(temp);
}


/*
    функция перерисовки экрана если курсор пытается выйти за пределы установленной 
    высоты
*/

void rewrite_display(char current_str[], int *num_str, int *x, int *y, char mode[]){
    FILE *fp;
    int count_str = 0;
    char ch;
    int old_num_str = *num_str;
    int status;
    int double_new_line = 0;

    //считаем количество строк в файле
    if((fp = fopen("./temp/temp1.mini", "r"))!=NULL){
        while((ch = fgetc(fp))!= EOF){
            if(ch == '\n'){
                if(!double_new_line){
                    count_str += 1;
                    double_new_line = 1;
                }    
                else{
                    double_new_line = 0;
                }
            }
            else{
                double_new_line = 0;
            }        
        }
        fclose(fp);
    }
    
    //перересовываем экран при нажатии кнопки вниз
    if(mode == "down"){
        if(*num_str < count_str-1){
            *num_str -= SCREEN_WORK_HEIGHT-2;
            for(int i = SCREEN_TOP-1; i < SCREEN_WORK_HEIGHT; i++){
                *y = 0; 
                *x = i;    
                *num_str += 1;
                status = read_current_str_from_file(current_str, *num_str, *x, y, mode);
                    
            }
            *x = SCREEN_WORK_HEIGHT;
            if(*num_str < count_str){
                *num_str = old_num_str+1;
            }
        }
    }
    //перересовываем экран при нажатии кнопки вверх
    else if(mode == "up"){
        if(*num_str > 0){
            *num_str += (SCREEN_WORK_HEIGHT - SCREEN_TOP + 1);
            for(int i = SCREEN_WORK_HEIGHT+1; i > SCREEN_TOP; i--){
                *y = 0; 
                *x = i;    
                *num_str -= 1;
                status = read_current_str_from_file(current_str, *num_str, *x, y, mode);
                    
            }

            *x = SCREEN_TOP;
            *num_str = old_num_str-1;
        }
        
    }
        
    gotoxy(*x, *y);    
    
}

/*
    функция для обновления содержимого экрана для строк, следующих за num_str
*/
void read_str_and_display(int num_str, int *x, int *y){
    int old_x = *x;
    int old_y = *y;
    int status;
    char current_str[SCREEN_WIDTH] = {'\0'};

    for(int i = old_x+1; i < SCREEN_WORK_HEIGHT+1; i++){
        clear_string_on_display(i);
        status = read_current_str_from_file(current_str, num_str, *x, y, "down");
        if(status == 1){
            *x += 1;
            num_str += 1;
        } 
       gotoxy(*x, *y);
    }
    *x = old_x;
    *y = old_y;

    gotoxy(*x, *y);
}

/*
    функция для добавления новой строки и перерисовки экрана (если номер строки
    превышает высоту ввода/вывода для редактора)
*/
void down_new_line(char current_str[], int *num_str, int *x, int *y){
    FILE *fp;
    int count_str = 0;
    int old_num_str = *num_str;
    char ch;
    int double_new_line = 0;
    if((fp = fopen("./temp/temp1.mini", "r"))!=NULL){
        while((ch = fgetc(fp))!= EOF){
            if(ch == '\n'){
                if(!double_new_line){
                    count_str += 1;
                    double_new_line = 1;
                }    
                else{
                    double_new_line = 0;
                }
            }
            else{
                double_new_line = 0;
            }        
        }
        fclose(fp);
    }

    if(*num_str >= count_str-1){
        *num_str -= (SCREEN_WORK_HEIGHT - SCREEN_TOP)+1;        
        if(old_num_str == count_str-1){
            *num_str += 1;
        }
        int status;
        for(int i = SCREEN_TOP-1; i < SCREEN_WORK_HEIGHT; i++){
            *y = 0; 
            *x = i;    
            
            status = read_current_str_from_file(current_str, *num_str, *x, y, "down");            
            *num_str += 1;
        }
        *x = SCREEN_WORK_HEIGHT;
        
        *num_str = old_num_str;            
        
        clear_string_on_display(*x);
        clear_current_str(current_str);    

    }
    else{
        *num_str -= (SCREEN_WORK_HEIGHT - SCREEN_TOP)+1;        
        
        int status;
        for(int i = SCREEN_TOP-1; i < SCREEN_WORK_HEIGHT; i++){
            *y = 0; 
            *x = i;    
            *num_str += 1;
            status = read_current_str_from_file(current_str, *num_str, *x, y, "down");            
        }
        *x = SCREEN_WORK_HEIGHT;
        *num_str = old_num_str;

    }
        
        gotoxy(*x, *y);
        
}

/*
    Навигация в редакторе при помощи стрелок
*/

void handle_navigation(char current_str[], int *num_str, char ch, int *x, int *y){
    int status;
    
    //вверх
    if(ch == 'A'){
        save_string(current_str, *num_str, *x, *y);
        if(*x > SCREEN_TOP){
            status = read_current_str_from_file(current_str, *num_str, *x, y, "up");
            if(status == 1){
                *x -= 1;
                *num_str -= 1;
            } 
            gotoxy(*x, *y);
        }
        else if((*x == SCREEN_TOP) && *num_str > 0){
            rewrite_display(current_str, num_str, x, y, "up");
        }
    } 
    //влево
    if(ch == 'D'){
        if(*y > 1){
            *y -= 1;
            gotoxy(*x, *y);
        }
        else if(*y == 1 && *x > SCREEN_TOP){
            status = read_current_str_from_file(current_str, *num_str, *x, y, "up");
            if(status == 1){
                *x -= 1;
                *num_str -= 1;
            } 
            gotoxy(*x, *y);
        }
    } 
    //вправо
    if(ch == 'C'){
        if(*y < SCREEN_WIDTH-1){
            *y += 1;
            gotoxy(*x, *y);
        }
        else if(*y == SCREEN_WIDTH-1 && *x < SCREEN_WORK_HEIGHT){
            *y = 0;
            *x += 1;
            *num_str += 1;
            gotoxy(*x, *y);
        }
    } 
    //вниз
    if(ch == 'B'){
        save_string(current_str, *num_str, *x, *y);
        if(*x <= (SCREEN_WORK_HEIGHT - 1)){           
            status = read_current_str_from_file(current_str, *num_str, *x, y, "down");
            if(status == 1){
                *x += 1;
                *num_str += 1;
            } 
            gotoxy(*x, *y);
        }
        else{
            rewrite_display(current_str, num_str, x, y, "down");

        }
    } 

}

void handle_esc_symbols(char current_str[], int *num_str, char ch, int *x, int *y){
    //обработка навигации
    handle_navigation(current_str, num_str, ch, x, y);
}

/*
    функция для обработки ввода символа новой строки
*/

void handle_new_string_symbol(char current_str[], int *num_str, int *x, int *y){
    int status;
    save_string(current_str, *num_str, *x, *y);    
    if(*x >= SCREEN_WORK_HEIGHT){
        down_new_line(current_str, num_str, x, y);
        *num_str += 1;
    }
    else{
        status = read_current_str_from_file(current_str, *num_str, *x, y, "down");
        *num_str += 1;
        *x += 1;
        if(status != 1){
            *y = 0;
        }
        gotoxy(*x, *y);
    }
    
}

/*
     функция для удаления строки из файла
*/
void delete_string_from_file(int num_str, int x, int y){
    FILE *fp, *fp2;
    char ch;

    if((fp2 = fopen("./temp/temp2.mini", "w"))==NULL){
            show_info_message("Ошибка открытия файла temp2.", x, y);
            return;
        }

        if((fp = fopen("./temp/temp1.mini", "r"))==NULL){
            show_info_message("Ошибка открытия файла temp1.", x, y);
            return;
        }
        
        while((ch = getc(fp)) != EOF){
            putc(ch, fp2);
        }

        fclose(fp2);
        fclose(fp);

        
        if((fp2 = fopen("./temp/temp2.mini", "r"))==NULL){
            show_info_message("Ошибка открытия файла temp2.", x, y);
            return;
        }

        if((fp = fopen("./temp/temp1.mini", "w"))==NULL){
            show_info_message("Ошибка открытия файла temp1.", x, y);
            return;
        }
        char str[SCREEN_WIDTH];
        int count_str = 0;
        int double_new_line = 0;
        int str_is_find = 0;

        //необходимо пропустить данное сочентание \n строка \n
        //остальное оставляем неизменным
        while(!feof(fp2)){
            if(fgets(str, SCREEN_WIDTH, fp2)){
                if(count_str == num_str){
                    count_str++;
                    double_new_line = 0;
                    str_is_find = 1;
                    continue;
                }
                else{
                    if(str_is_find){
                        str_is_find = 0;
                        continue;
                    }
                    if(!double_new_line){
                        count_str++;
                        double_new_line = 1;
                    }
                    else{
                        double_new_line = 0;
                    }            
                    fputs(str, fp);
                }
            }
        }
    fclose(fp);
}

/*
    функция для чтения каждого символа при вводе (без буфферизации)
*/

struct termios def_settings;

char mygetch() {
    //настройки терминала
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    tcgetattr(STDIN_FILENO, &def_settings);
    newt = oldt;
    //устанавливаем новые настройки (без буффера)
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    //возвращаем прежние настройки
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

/*
    функция обработки ctrl+C
*/
void  INThandler(int sig)
{
    //возвращаем прежние настройки, очищаем экран и выходим
    signal(sig, SIG_DFL);
    tcsetattr(STDIN_FILENO, TCSANOW, &def_settings);
    if (sig == 2){
        clear_temp_dir();
        clear_all();
        exit(0);
    }
}


int main(int argc, char *argv[])
{
    signal(SIGINT, INThandler);
    signal(SIGTERM, INThandler);

    //переменные для манипуляции курсора на экране
    int x = SCREEN_TOP;
    int y = 0;

    char ch;
    char code;
    int spec_code;
    int is_escape = 1;

    //массив сиволов - строка на экране
    char current_str[SCREEN_WIDTH] = {0};
    //номер строки
       int num_str = 0;

       char *filename = NULL;

    if(argc > 1){
        clear_all();
        create_canvas(x, y);
        //создаем и очищаем временное хранилище
        create_temp_dir();
        clear_temp_dir();

        //открываем файл переданный при вызове программы
        FILE *fp;
        if((fp = fopen(argv[1], "r"))==NULL){
            printf("Файл не найден!\n");
            exit(1);
        }
        //читаем файл и выводим на экран
        read_file_and_show(fp, current_str, &x, &y, &num_str);
        fclose(fp);
        filename = argv[1];
    }
    else{
        printf("Файл не указан\n");
        exit(0);
    }

    while(1){

        ch = mygetch();

            //удалить символ
            if(ch == 8 || ch == 127){
                //переходим в текущую ячейку и ставим пробельный символ на экране
                if(y > 0){
                    gotoxy(x, y);
                    putchar(' ');
                    y -= 1;
                    
                }
                else if(y == 0 && x > SCREEN_TOP){
                    //удаляем строку из файла и перерисовываем экран
                    delete_string_from_file(num_str, x, y);
                    int status = read_current_str_from_file(current_str, num_str, x, &y, "up");
                    if(status == 1){
                        x -= 1;
                        num_str -= 1;
                       
                    } 
                    read_str_and_display(num_str, &x, &y);
                    
                }
                else{
                    continue;
                }
                
                gotoxy(x, y+1);
                if(current_str[y+1] == '\0'){
                    current_str[y] = '\0';
                }
                else{
                    current_str[y] = ' ';
                }
                
                continue;
            } 
            //esc-символ
            if(ch == '\033'){           
                ch = mygetch();
                
                if(ch == 91){
                    ch = getc(stdin);
                    handle_esc_symbols(current_str, &num_str, ch, &x, &y);
                    continue;                
                }
                else{
                    break;
                }             
            }
            //новая строка 
            if(ch == '\n'){    
                handle_new_string_symbol(current_str, &num_str, &x, &y);                    
                continue;
            }

            //ctrl+O
            if(ch == 15){
                if(filename != NULL){
                    save_file(filename, x, y);
                    show_info_message("Save", x, y);
                }
                else{
                    show_info_message("Save file?", x, y);
                }
            }
            //acsii-символ
            if(ch > 31 && ch < 128){
                if(y == SCREEN_WIDTH-2){
                    handle_new_string_symbol(current_str, &num_str, &x, &y);                 
                    continue;    
                }
                y += 1;
                for(int i = 0; i < y; i++){
                        if(current_str[i] == '\0'){
                            current_str[i] = ' ';
                            current_str[i+1] = '\0';
                        }
                    }
                if(y != 0)
                current_str[y-1] = ch;

                if(!check_null_symbol(current_str, y)){
                    current_str[y] = '\0';
                }
                gotoxy(x, y);
                putchar(ch);
            }    

    }
    clear_all();
    return 0;
}