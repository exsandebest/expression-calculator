#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>


struct Node{
    QString type;
    double num_val;
    QString operation_val;
    QString variable_val;
    Node * next;
};


struct Pair {
    int idx;
    double result;
};

struct Pair_string{
    int idx;
    QString result;
};


class Queue {
public:
    Node* head;
    Node * tail;
    Queue(){
        head = nullptr;
        tail = nullptr;
    }
    void push(QString type, double num = 0, QString operation_val = "", QString variable_val = ""){
        Node * new_elem = new Node;
        new_elem->type = type;
        new_elem->num_val = num;
        new_elem->operation_val = operation_val;
        new_elem->variable_val = variable_val;
        new_elem->next = nullptr;
        if (tail == nullptr){
            tail = new_elem;
            head = tail;
        } else {
            tail->next = new_elem;
            tail = tail->next;
        }
    }
    void push_num(double num_val){
        Node * new_elem = new Node;
        new_elem->type = "num";
        new_elem->num_val = num_val;
        new_elem->variable_val = "";
        new_elem->operation_val = "";
        new_elem->next = nullptr;
        if (tail == nullptr){
            tail = new_elem;
            head = tail;
        } else {
            tail->next = new_elem;
            tail = tail->next;
        }
    }

    void push_elem(Node * elem){
        Node * new_elem = new Node;
        new_elem->type = elem->type;
        new_elem->num_val = elem->num_val;
        new_elem->operation_val = elem->operation_val;
        new_elem->variable_val = elem->variable_val;
        new_elem->next = nullptr;
        if (tail == nullptr){
            tail = new_elem;
            head = tail;
        } else {
            tail->next = new_elem;
            tail = tail->next;
        }
    }

    void push_variable(QString variable_val){
        Node * new_elem = new Node;
        new_elem->type = "variable";
        new_elem->variable_val = variable_val;
        new_elem->num_val = 0;
        new_elem->operation_val = "";
        new_elem->next = nullptr;
        if (tail == nullptr){
            tail = new_elem;
            head = tail;
        } else {
            tail->next = new_elem;
            tail = tail->next;
        }
    }

    int size(){
        if (tail == nullptr && tail == head) return 0;
        Node * tmp = head;
        int ans = 1;
        while (tmp != tail){
            ++ans;
            tmp = tmp->next;
        }
        return ans;
    }

    void push_operation(QString operation_val){
        Node * new_elem = new Node;
        new_elem->type = "operation";
        new_elem->operation_val = operation_val;
        new_elem->num_val = 0;
        new_elem->variable_val = "";
        new_elem->next = nullptr;
        if (tail == nullptr){
            tail = new_elem;
            head = tail;
        } else {
            tail->next = new_elem;
            tail = tail->next;
        }
    }



    Node * pop () {
        if (head == nullptr){
            return nullptr;
        }
        Node * elem = new Node;
        elem->next = nullptr;
        elem->type = head->type;
        elem->num_val = head->num_val;
        elem->operation_val = head->operation_val;
        elem->variable_val = head->variable_val;
        head = head->next;
        if (head == nullptr) tail = nullptr;
        return elem;
    }
    bool is_empty(){
        return (head == nullptr);
    }
    void clear(){
        while (!this->is_empty()){
            Node * q = this->pop();
            delete (q);
        }
        head = tail = nullptr;
    }
    void print(){
        qDebug() << "Print:";
                Node * elem = head;
                while (elem != nullptr){
                    if (elem->type == "num"){
                        qDebug() << elem->num_val;
                    } else if (elem->type == "operation"){
                        qDebug() << elem->operation_val;
                    } else if (elem->type == "variable"){
                        qDebug() << elem->variable_val;
                    }
                    elem = elem->next;
                }
    }
    Node * last_in(){
        if (tail == nullptr){
            return nullptr;
        }
        Node * elem = new Node;
        elem->next = nullptr;
        elem->type = tail->type;
        elem->num_val = tail->num_val;
        elem->operation_val = tail->operation_val;
        elem->variable_val = tail->variable_val;
        return elem;
    }

};



class Stack {
    public:
    Node * head;
    Stack() {
        head = nullptr;
    }
    void push(QString type, double num = 0, QString operation_val = "", QString variable_val = ""){

        Node * new_elem = new Node;
        new_elem->type = type;
        new_elem->num_val = num;
        new_elem->operation_val = operation_val;
        new_elem->variable_val = variable_val;
        new_elem->next = head;
        head = new_elem;
    }

    void push_elem(Node * elem){
        Node * new_elem = new Node;
        new_elem->type = elem->type;
        new_elem->num_val = elem->num_val;
        new_elem->operation_val = elem->operation_val;
        new_elem->variable_val = elem->variable_val;
        new_elem->next = head;
        head = new_elem;
    }


    void push_num(double num_val){
        Node * new_elem = new Node;
        new_elem->type = "num";
        new_elem->variable_val = "";
        new_elem->operation_val = "";
        new_elem->num_val = num_val;
        new_elem->next = head;
        head = new_elem;
    }

    void push_operation(QString operation_val){
        Node * new_elem = new Node;
        new_elem->type = "operation";
        new_elem->operation_val = operation_val;
        new_elem->num_val = 0;
        new_elem->variable_val = "";
        new_elem->next = head;
        head = new_elem;
    }
    void push_variable(QString variable_val){
        Node * new_elem = new Node;
        new_elem->type = "variable";
        new_elem->variable_val = variable_val;
        new_elem->operation_val = "";
        new_elem->num_val = 0;
        new_elem->next = head;
        head = new_elem;
    }


    Node * pop () {
        if (head == nullptr){
            return nullptr;
        }
        Node * elem = new Node;
        elem->next = nullptr;
        elem->type = head->type;
        elem->num_val = head->num_val;
        elem->operation_val = head->operation_val;
        elem->variable_val = head->variable_val;
        head = head->next;
        return elem;
    }
    Node * top(){
        if (head == nullptr){
            return nullptr;
        }
        Node * elem = new Node;
        elem->next = nullptr;
        elem->type = head->type;
        elem->num_val = head->num_val;
        elem->operation_val = head->operation_val;
        elem->variable_val = head->variable_val;
        return elem;
    }

    bool is_empty(){
        return (head == nullptr);
    }

    void clear(){
        while (!this->is_empty()){
            Node * q = this->pop();
            delete (q);
        }
        head = nullptr;
    }

    void print(){
        qDebug() << "Print:";
                Node * elem = head;
                while (elem != nullptr){
                    if (elem->type == "num"){
                        qDebug() << elem->num_val;
                    } else if (elem->type == "operation"){
                        qDebug() << elem->operation_val;
                    } else if (elem->type == "variable"){
                        qDebug() << elem->variable_val;
                    }
                    elem = elem->next;
                }
    }


};


void question_button_clicked(QObject * obj);

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Queue const_to_num(Queue q);
    Queue Dijkstra(Queue q);
    int get_priority(QString operation);
    double evaluate(Queue input);
    int get_argumets_amount(QString operation);
    double calculate (QString s, double x, double y);

    Queue get_variables_from_queue();
    void get_variables_from_input();
    void write_ans(double ans);
    Queue get_new_queue();

    double sin (double x);
    double cos (double x);
    double ln(double a);
    double log(double a, double b);
    double pow (double x, double a);
    double pow_simple(double x, long long a);
    double exp(double x);



    bool checkCBS(QString s);
    bool try_evaluate(Queue q);
    void validate_log(Queue q);
    bool validate_comma(QString &str);
    Queue validate_function(Queue q);
    void validate_space(QString &str);

    void parse(QString &str, int idx);
    Pair get_num(QString &str, int idx);
    Pair make_pair(double result, int idx);
    int operation_or_variable(QString &str, int idx);
    Pair_string get_var(QString &str, int idx);



    void keyPressEvent(QKeyEvent* event);

    bool equal(double a, double b);

    void output();
    QString z_to_s(double a);
private slots:

    void update_ans(int row, int col);

    void on_btn_enter_clicked();

    void on_btn_angle_clicked();

    void on_theme_1_btn_clicked();

    void on_theme_2_btn_clicked();

    void on_theme_3_btn_clicked();

    void on_theme_4_btn_clicked();

    void on_btn_question_clicked();

    void on_btn_stop_clicked();

    void question_slot() {
        question_button_clicked(sender());
    }



private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
