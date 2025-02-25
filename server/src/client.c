#include <stdio.h>
#include <gtk/gtk.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#include "../libs/protocol.h"
#include "../libs/tool.h"
#include "../libs/valid.h"

#define BUFF_SIZE 1024

// Question *ques = (Question *)malloc(sizeof(Question));
// Request *request = (Request *)malloc(sizeof(Request));
// Response *response = (Response *)malloc(sizeof(Response));
// Information *infor = (Information *)malloc(sizeof(Information));

// gameplay for normal client
void gamePlayForNormalTutorial()
{
    printf("-------------------Đấu trường 100-------------------\n");
    printf("\nGameplay Tutorial(Choose answer): ");
    printf("\n\tAnswer syntax: ANSWER answer");
    printf("\n-------------------Đấu trường 100-------------------");
    printf("\nInput to syntax: \n");
}
// tutorial choose topic
void chooseTopicLevel()
{
    printf("-------------------Đấu trường 100-------------------\n");
    printf("\nGameplay Tutorial (Choose level): ");
    printf("\n\tChoose Topic Level syntax: TOPIC level (level: EASY, NORMAL, HARD)");
    printf("\n--------------------------------------------------");
    printf("\nInput to syntax: \n");
}
// game play for special player
void gamePlayForSpecialTutorial()
{
    printf("-------------------Đấu trường 100-------------------\n");
    printf("\nGameplay Tutorial(Choose answer): ");
    printf("\n\tChoose Answer syntax: ANSWER answer");
    printf("\n\tUse Help syntax: HELP");
    printf("\n--------------------------------------------------");
    printf("\nInput to syntax: \n");
}
void showQuestion(Question *question)
{
    printf("\n%s", question->question);
    printf("\n%s", question->answer1);
    printf("\n%s", question->answer2);
    printf("\n%s", question->answer3);
    printf("\n%s", question->answer4);
}
GtkWidget *username_widget;
GtkWidget *password_widget;
GtkWidget *index_;
int choose_lucky = 0;

GAMEPLAY_STATUS status = UNAUTH;
int clickedSubmit = 0;
int client_sock, servPort;
GtkWidget *Login_Username;
int lucky;
char name[30], luckyPlayer[BUFF_SIZE], topic[BUFF_SIZE];
Question *ques = NULL;
Request *request = NULL;
Response *response = NULL;
Information *infor = NULL;
char buff[BUFF_SIZE];
char pass[30];
char register_line[BUFF_SIZE];
char buff1[BUFF_SIZE];
int information = TRUE;
int help = FALSE;
GtkWidget *Login_Password;
GtkWidget *GameView;
GtkWidget *TopicChoice;
GtkWidget *GameView1;
GtkWidget *waiting_window;
GtkWidget *waiting_mess;
GtkWidget *Register;
GtkWidget *user_regis;
GtkWidget *pass_regis;

void clickedToLogin(GtkButton *login, gpointer data)
{
    GtkBuilder *builder;
    builder = gtk_builder_new_from_file("/home/thien/dau-truong-100/View.glade");
    Login_Username = GTK_WIDGET(gtk_builder_get_object(builder, "Login_Username"));
    gtk_builder_connect_signals(builder, NULL);
    username_widget = GTK_WIDGET(gtk_builder_get_object(builder, "username"));
    g_signal_connect(Login_Username, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_object_unref(builder);
    gtk_widget_show(Login_Username);
}

void clickedToRegister(GtkButton *regist, gpointer data)
{
    GtkBuilder *builder;
    builder = gtk_builder_new_from_file("/home/thien/dau-truong-100/View.glade");
    Register = GTK_WIDGET(gtk_builder_get_object(builder, "Register"));
    gtk_builder_connect_signals(builder, NULL);
    user_regis = GTK_WIDGET(gtk_builder_get_object(builder, "user_regis"));
    pass_regis = GTK_WIDGET(gtk_builder_get_object(builder, "pass_regis"));
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(Register, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_object_unref(builder);
    gtk_widget_show(Register);
}

void clickedToResigerSubmit(GtkButton *subm, gpointer data)
{
    strcpy(register_line, "REGISTER ");
    strcat(register_line, gtk_entry_get_text(user_regis));
    strcat(register_line, " ");
    strcat(register_line, gtk_entry_get_text(pass_regis));
    setOpcodeRequest(request, register_line);
    sendRequest(client_sock, request, sizeof(Request), 0);
    receiveResponse(client_sock, response, sizeof(Response), 0);
    strcpy(buff, readMessageResponse(response));
    show_message(Register, GTK_MESSAGE_INFO, "Thong bao", buff);
    gtk_widget_hide(Register);
}

void clickedToUserSubmit(GtkButton *Submit, gpointer data)
{
    strcpy(name, "USER ");
    strcat(name, gtk_entry_get_text(username_widget));
    request = (Request *)malloc(sizeof(Request));
    response = (Response *)malloc(sizeof(Response));
    setOpcodeRequest(request, name);
    sendRequest(client_sock, request, sizeof(Request), 0);
    receiveResponse(client_sock, response, sizeof(Response), 0);
    strcpy(buff, readMessageResponse(response));
    status = response->status;
    GtkBuilder *builder;
    builder = gtk_builder_new_from_file("/home/thien/dau-truong-100/View.glade");
    Login_Password = GTK_WIDGET(gtk_builder_get_object(builder, "Login_Password"));
    gtk_builder_connect_signals(builder, NULL);
    password_widget = GTK_WIDGET(gtk_builder_get_object(builder, "password"));
    buff[strlen(buff) - 1] = '\0';
    if (strcmp(buff, "Username is correct ") != 0)
    {
        show_message(Login_Username, GTK_MESSAGE_INFO, "Thong bao", buff);
    }
    else
    {
        gtk_widget_hide(index_);
        gtk_widget_show(Login_Password);
    }
    g_signal_connect(Login_Password, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_object_unref(builder);
}

void show_message(GtkWidget *parent, GtkMessageType type, char *mms, char *content)
{
    GtkWidget *mdialog;
    mdialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     type,
                                     GTK_BUTTONS_OK,
                                     "%s", mms);
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(mdialog), "%s", content);
    gtk_dialog_run(GTK_DIALOG(mdialog));
    gtk_widget_destroy(mdialog);
}

void clickedToPassSubmit(GtkButton *Submit, gpointer data)
{
    strcpy(pass, "PASS ");
    strcat(pass, gtk_entry_get_text(password_widget));
    request = (Request *)malloc(sizeof(Request));
    response = (Response *)malloc(sizeof(Response));
    setOpcodeRequest(request, pass);
    sendRequest(client_sock, request, sizeof(Request), 0);
    receiveResponse(client_sock, response, sizeof(Response), 0);
    strcpy(buff, readMessageResponse(response));
    if (response->code == PASSWORD_CORRECT)
        gtk_widget_hide(Login_Username);
    status = response->status;
    strcpy(buff1, buff);
    buff[strlen(buff) - 1] = '\0';
    GtkBuilder *builder;
    GtkWidget *yourname;
    builder = gtk_builder_new_from_file("/home/thien/dau-truong-100/View.glade");
    waiting_window = GTK_WIDGET(gtk_builder_get_object(builder, "waiting_window"));
    gtk_builder_connect_signals(builder, NULL);
    waiting_mess = GTK_WIDGET(gtk_builder_get_object(builder, "waiting_mess"));
    yourname = GTK_WIDGET(gtk_builder_get_object(builder, "yourname"));
    if (response->status == WAITING_PLAYER)
    {
        gtk_entry_set_text(GTK_ENTRY(waiting_mess), "Waiting other player... ");
        gtk_label_set_text(GTK_LABEL(yourname), name);
        gtk_widget_show(waiting_window);
        g_signal_connect(waiting_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        g_object_unref(builder);
        gtk_widget_hide(Login_Password);
    }

    else
        show_message(Login_Password, GTK_MESSAGE_INFO, "Thong bao", buff1);
}

void clickedToWaitingOk(GtkButton *Ok, gpointer data)
{
    if(choose_lucky == 0){
        requestGet(client_sock);
        receiveResponse(client_sock, response, sizeof(Response), 0);
        if (response->status == WAITING_QUESTION)
        {
            memset(luckyPlayer, '\0', (strlen(luckyPlayer) + 1));
            strcpy(luckyPlayer, response->data);
            printf("%s\n", luckyPlayer);
            if (strcmp(luckyPlayer, gtk_entry_get_text(username_widget)) == 0)
                lucky = TRUE;
            else
                lucky = FALSE;
        }
        choose_lucky = 1;
    }
    if (response->status == WAITING_QUESTION && choose_lucky == 1)
    {
        infor = (Information *)malloc(sizeof(Information));
        if (lucky == TRUE)
        {

            if (information == FALSE)
            {
                requestCheckInformation(client_sock);
                receiveInformation(client_sock, infor, sizeof(Information), 0);
                if (infor->status == TRUE)
                {
                    information = TRUE;
                    if (help == FALSE)
                    {
                    }
                    else
                    {
                    }
                }
            }
            else
            {
                requestGet(client_sock);
                receiveResponse(client_sock, response, sizeof(Response), 0);
                information = FALSE;
                if (response->status == END_GAME)
                {
                    status = response->status;
                    //readMessageResponse(response);
                }
                else
                {
                    GtkBuilder *builder;
                    builder = gtk_builder_new_from_file("/home/thien/dau-truong-100/View.glade");
                    TopicChoice = GTK_WIDGET(gtk_builder_get_object(builder, "TopicChoice"));
                    gtk_builder_connect_signals(builder, NULL);
                    g_signal_connect(TopicChoice, "destroy", G_CALLBACK(gtk_main_quit), NULL);
                    g_object_unref(builder);
                    gtk_widget_show(TopicChoice);
                }
            }
        }
        else
        {
            // check status of game: playing or end?
            GtkBuilder *builder;
            builder = gtk_builder_new_from_file("/home/thien/dau-truong-100/View.glade");
            GameView1 = GTK_WIDGET(gtk_builder_get_object(builder, "GameView1"));
            gtk_builder_connect_signals(builder, NULL);
            g_signal_connect(GameView1, "destroy", G_CALLBACK(gtk_main_quit), NULL);
            g_object_unref(builder);
            gtk_widget_show(GameView1);
        }
    }
    else
    {
        gtk_entry_set_text(GTK_ENTRY(waiting_mess), "Waiting other player... ");
    }
}
int main(int argc, char const *argv[])
{
    char username[BUFF_SIZE], topic[BUFF_SIZE];
    struct sockaddr_in server_addr; /* server's address information */
    int msg_len, bytes_sent, bytes_received;
    char buff[BUFF_SIZE], code[BUFF_SIZE], data[BUFF_SIZE];
    ques = (Question *)malloc(sizeof(Question));
    request = (Request *)malloc(sizeof(Request));
    response = (Response *)malloc(sizeof(Response));
    Information *infor = (Information *)malloc(sizeof(Information));
    int lucky = FALSE, existQuestion = FALSE, help = FALSE;
    int questionNumber = 0;
    float score = 0;
    int inforamation = TRUE;
    int gameStatus = GAME_PLAYING;

    // gtk
    GtkBuilder *builder;

    if (argc != 3)
    {
        printf("\nParams incorrect\n");
    }
    else
    {
        // Check input : IP address & Port
        if (checkIPAndPort(argv[1], argv[2]) != 0)
        {
            // Step 1: Construct socket
            client_sock = socket(AF_INET, SOCK_STREAM, 0);
            // Step 2: Specify server address
            servPort = atoi(argv[2]);

            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(servPort);
            server_addr.sin_addr.s_addr = inet_addr(argv[1]);
            // Step 3: Request to connect server
            if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
            {
                printf("\nError!Can not connect to sever! Client exit imediately! ");
                return 0;
            }
            gtk_init(&argc, &argv);

            builder = gtk_builder_new_from_file("/home/thien/dau-truong-100/View.glade");

            index_ = GTK_WIDGET(gtk_builder_get_object(builder, "Index"));
            gtk_builder_connect_signals(builder, NULL);

            g_signal_connect(index_, "destroy", G_CALLBACK(gtk_main_quit), NULL);

            g_object_unref(builder);

            gtk_widget_show(index_);
            gtk_main();
        }
    }
}
