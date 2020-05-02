#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <math.h>
#include <QTableWidget>
#include <QAbstractItemDelegate>
#include <QMessageBox>
#include <QKeyEvent>
#include <QMovie>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>

QMovie *movieNew;
int pause_flag;
int question_button_flag;
QWidget *question_widget;
QLabel *question_label;
QGridLayout *question_layout;
QPushButton *question_button;
Queue que_post;
Queue variables;
const double pi = 3.141592653589793238462643383279;
Queue que_in;
const double e = 2.718281828459045235360287471352;
const double PRECISION = 1e-15;
int angle = 1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->table_widget->setColumnCount(2);
    QTableWidgetItem * itm1 = new QTableWidgetItem, * itm2 = new QTableWidgetItem;
    itm1->setText("Переменная");
    itm2->setText("Значение");
    ui->table_widget->setHorizontalHeaderItem(0,itm1);
    ui->table_widget->setHorizontalHeaderItem(1,itm2);
    ui->table_widget->horizontalHeader()->resizeSection(0,134);
    ui->table_widget->horizontalHeader()->resizeSection(1,135);
    ui->table_widget->verticalHeader()->setVisible(false);
    pause_flag = 0;
    question_widget = new QWidget;
    question_layout = new QGridLayout;
    question_widget->setLayout(question_layout);
    question_label = new QLabel;
    question_layout->addWidget(question_label, 0, 0);
    question_button = new QPushButton;
    question_layout->addWidget(question_button, 1, 0);
    question_button_flag = 0;
    connect(question_button, SIGNAL(clicked()), this, SLOT(question_slot()));
    question_button_clicked(this);


    ui->theme_1_btn->setStyleSheet({"background-image: url(\":/other/icon_1.png\");"});
    ui->theme_2_btn->setStyleSheet({"background-image: url(\":/other/icon_2.png\");"});
    ui->theme_3_btn->setStyleSheet({"background-image: url(\":/other/icon_3.png\");"});
    ui->theme_4_btn->setStyleSheet({"background-image: url(\":/other/icon_4.png\");"});
    on_theme_1_btn_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}
double MainWindow::sin (double x) {
    double ans = 0, tmp = x;
    for(long long i = 1; fabs(tmp) >= PRECISION ; ++i ){
      ans += tmp;
      tmp *= -x*x/((2.0*i)*(2*i+1));
    }
    return ans;
}


double MainWindow::cos (double x) {
    double ans = 1, tmp = 1;
    for(long long i = 1; fabs(tmp) >= PRECISION ; ++i ){
      tmp *= -x*x/((2.0*i)*(2*i-1));
      ans += tmp;
    }
    return ans;
}

double MainWindow::ln(double a) {
    int k = 0;
    while (a > e+PRECISION){
        a /= e;
        ++k;
    }
    double x = (a - 1)/(a + 1);
    double ans = x;
    double tmp = 1;
    for (double i = 3; fabs(tmp)>=PRECISION; i+=2) {
        tmp = pow_simple(x,i)/i;
        ans += tmp;
    }
    return 2*ans + k;
}


double MainWindow::exp(double x){
    double ans = 1;
    double tmp = 1;
    for (int i = 1; fabs(tmp) >= PRECISION; ++i){
        tmp *= x/i;
        ans += tmp;
    }
    return ans;
}


void MainWindow::validate_log(Queue q){
    int sz = q.size();
    Stack s;
    int state = 0;
    for (int i = 0 ; i < sz; ++i){
        Node * elem = q.pop();
            if (elem->operation_val == "log"){
                q.push_elem(elem);
                elem = q.pop();
                s.push_operation("{");
                state = 1;
            } else if (elem->operation_val == "(" || elem->operation_val == "["){
                s.push_operation(elem->operation_val);
            } else if (elem->operation_val == ")" || elem->operation_val == "]"){
                Node * tmp = s.pop();
                if (!s.is_empty() && (s.top()->operation_val == "{")){
                    if (tmp->operation_val != ",") {
                        throw QString ("Функция log должна иметь два аргумента, рахделеных запятой");
                    } else {
                        s.pop();
                        state = 0;
                    }
                }
            } else if (elem->operation_val == ","){
                s.push_operation(",");
            } else if (elem->operation_val == "log" && state == 1){
                q.push_elem(elem);
                validate_log(q);
                elem = q.pop();
            }
        q.push_elem(elem);
    }
}







bool MainWindow::equal(double a, double b){
    return (fabs(a-b) < PRECISION);
}






double MainWindow::pow_simple(double x, long long a){
    if (a == 0) return 1;
    if (a == 1) return x;
    if (a < 0) return 1.0/pow_simple(x,-a);
    double res = 1;
    while (a){
        if (a & 1){
            res *= x;
            --a;
        } else {
            x *= x;
            a >>= 1;
        }
    }
    return res;

}

double MainWindow::pow(double x, double a) {
    if (x < 0 && !equal(a, round(a))){
        throw QString("Нельзя возводить отрицательные числа в дробную степень");
    } else if (equal(x, 0) && a < 0){
        throw QString("Нельзя возводить 0 в отрицательную степень");
    } else if (equal(x, 0) && equal(a, 0)){
        throw QString("Выражение 0^0 не определено");
    } else if (equal(x, 1)){
        return 1;
    } else if (equal(x, 0)){
        return 0;
    } else if (equal(a, round(a))){
        return pow_simple(x,round(a));
    } else if (equal(a, round(a)) && equal(x, round(x))){
        if (a<0) {
            return 1.0/pow_simple(round(x),round(a));
        } else {
            return pow_simple(round(x),round(a));
        }
    }
    return exp(ln(x) * a);
}

double MainWindow::calculate(QString s, double x, double y){
       if (s == "+") return x + y;
       if (s == "-") return x - y;
       if (s == "*") return x*y;
       if (s == "/") {
           if (equal(y,0)){
               throw QString("Деление на 0 запрещено");
           }
           return x/y;
       }
       if (s == "^") return pow(x, y); //x^y
       if (s == "sin") {
           if (angle == 0){
               x = (pi/180) * x;
           }
           int k = 0;
           double tmp_x = x;
           if (tmp_x > 0){
             while (tmp_x > 2*pi){
                 ++k;
                 tmp_x -= 2*pi;
             }
           } else {
               while (tmp_x < -2*pi){
                   --k;
                   tmp_x += 2*pi;
               }
           }
         return sin(x-k*2*pi);
       }
       if (s == "cos") {
           if (angle == 0){
               x = (pi/180) * x;
           }
           int k = 0;
           double tmp_x = x;
           if (tmp_x > 0){
             while (tmp_x > 2*pi){
                 ++k;
                 tmp_x -= 2*pi;
             }
           } else {
               while (tmp_x < -2*pi){
                   --k;
                   tmp_x += 2*pi;
               }
           }
         return cos(x-k*2*pi);
       }
       if (s == "log") return log(x, y); //LOGa(X)
       if (s == "exp") return exp(x);
       if (s == "tg"){
           if (angle == 0){
               x = (pi/180) * x;
           }
           int k = 0;
           double tmp_x = x;
           if (tmp_x > 0){
             while (tmp_x > 2*pi){
                 ++k;
                 tmp_x -= 2*pi;
             }
           } else {
               while (tmp_x < -2*pi){
                   --k;
                   tmp_x += 2*pi;
               }
           }
           if (equal(tmp_x, pi/2) || equal(tmp_x, 3*pi/2) || equal(tmp_x, -pi/2) || equal(tmp_x, -3*pi/2)) throw QString ("Значение не входит в ОДЗ тангенса");
           return sin(x-k*2*pi)/cos(x-k*2*pi);
       }
       if (s == "ctg"){
           if (angle == 0){
               x = (pi/180) * x;
           }
           int k = 0;
           double tmp_x = x;
           if (tmp_x > 0){
             while (tmp_x > 2*pi){
                 ++k;
                 tmp_x -= 2*pi;
             }
           } else {
               while (tmp_x < -2*pi){
                   --k;
                   tmp_x += 2*pi;
               }
           }
           if (equal(tmp_x, 0) || equal(tmp_x, pi) || equal(tmp_x, 2*pi) || equal(tmp_x, -pi) || equal(tmp_x, -2*pi)) throw QString ("Значение не входит в ОДЗ котангенса");
           return cos(x-k*2*pi)/sin(x-k*2*pi);
       }
       if (s == "~") return -x;
       else return 0;
}


double MainWindow::log (double a, double b) {
    if (a <= 0 || equal(a,1) || b <= 0) throw QString("Значения не входят в ОДЗ функции логарифма");
    return ln(b)/ln(a);
}



Queue MainWindow::Dijkstra(Queue q){
    Stack s;
    Queue ans;
    while(!q.is_empty()){
        Node * elem = q.pop();
        if (elem->type == "num"){
            ans.push_num(elem->num_val);
        } else if (elem->type == "variable"){
            ans.push_elem(elem);
        }else if (elem->operation_val == "(" ||  elem->operation_val == "["){
            s.push_operation(elem->operation_val);
        } else {
            int current_priority = get_priority(elem->operation_val);

            int closing_bracket_comma = 0;
            if (elem->operation_val == ")" || elem->operation_val == "]") closing_bracket_comma = 1;
            if (elem->operation_val == ",") closing_bracket_comma = 2;
            if (closing_bracket_comma == 1){
                while (1){
                    if (s.top() == nullptr) break;
                    if (s.top()->operation_val == "(" || s.top()->operation_val == "["){
                        s.pop();
                        break;
                    }
                    Node * current_elem = s.pop();
                    ans.push_elem(current_elem);
                }
            } else if (closing_bracket_comma == 2) {
                while (1){
                    if (s.top() == nullptr) break;
                    if (s.top()->operation_val == "(" || s.top()->operation_val == "["){
                        break;
                    }
                    Node * current_elem = s.pop();
                    ans.push_elem(current_elem);
                }
            } else {
            while (1){
                if (s.top() == nullptr) break;
                if (s.top()->operation_val == "(") break;
                Node * current_elem = s.pop();
                if ((elem->operation_val != "^" && get_priority(current_elem->operation_val) >= current_priority) ||
                        (elem->operation_val == "^" &&  get_priority(current_elem->operation_val) > current_priority)){
                    ans.push_elem(current_elem);
                } else {
                    s.push_elem(current_elem);
                    break;
                }
            }
            s.push_elem(elem);
            }

        }
    }
    while (!s.is_empty()){
        Node * elem = s.pop();
        ans.push_elem(elem);
    }
    return ans;
}

int MainWindow::get_priority(QString operation){
    if (operation == "cos" || operation == "sin" || operation == "tg" || operation == "ctg" || operation == "log" || operation == "exp"){
        return 5;
    } else if (operation == "^"){
        return 3;
    }else if (operation == "*" || operation == "/"){
        return 2;
    } else if (operation == "+" || operation == "-"){
        return 1;
    } else if (operation == ")"){
        return 0;
    } else if (operation == "~"){
        return 4;
    } else if (operation == ",") {
        return 1;
    }
    return 0;
}


double MainWindow::evaluate(Queue input){
    Stack s;
    while(!input.is_empty()){
        Node * elem = input.pop();
        if (elem->type == "num"){
            s.push_num(elem->num_val);
        } else {
            int arguments_amount = get_argumets_amount(elem->operation_val);
            if (arguments_amount == 2){
                if(s.is_empty()) throw QString("Слишком мало операндов (Доминирование операций)");
                double num_1 = s.pop()->num_val;
                if(s.is_empty()) throw QString("Слишком мало операндов (Доминирование операций)");
                double num_2 = s.pop()->num_val;
                double ans = calculate(elem->operation_val, num_2, num_1);//Функция подсчета двух чисел
                s.push_num(ans);
            } else {
                if(s.is_empty()) throw QString("Слишком мало операндов (Доминирование операций)");
                double num_1 = s.pop()->num_val;
                double ans = calculate(elem->operation_val, num_1 , 0); // Функция подсчета одного числа
                s.push_num(ans);
            }
        }
    }
    double ans = s.pop()->num_val;
    if (!s.is_empty()) throw QString("Слишком мало операций (Доминирование операндов)");
    return ans;
}






void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        on_btn_enter_clicked();
    }
}



bool MainWindow::checkCBS(QString s){
    Stack st;
    for (int i = 0; i < s.length(); ++i){
        if (s[i] == "(" || s[i] == "["){
            st.push_operation(QString(s[i]));
            continue;
        }
        if (s[i] == ")"){
            if (st.is_empty()) return false;
            Node * tmp = st.pop();
            if (tmp->operation_val != "(") return false;
            continue;
        }
        if (s[i] == "]"){
            if (st.is_empty()) return false;
            Node * tmp = st.pop();
            if (tmp->operation_val != "[") return false;
            continue;
        }
    }
    if (st.is_empty()) return true;
    return false;
}




int MainWindow::get_argumets_amount(QString operation){
    if (operation == "sin" || operation == "cos" || operation == "tg" || operation == "ctg" || operation == "exp" || operation == "~"){
        return 1;
    } else {
        return 2;
    }
}

void MainWindow::output(){
    QString ans = "";
    int que_post_size = que_post.size();
    for (int i = 0 ; i < que_post_size; ++i){
        Node * elem = que_post.pop();
        if (elem->type == "num"){
            QString s1;
            /*if (equal(elem->num_val, round(elem->num_val))){
                s1 = z_to_s(round(elem->num_val));
            } else {*/
                s1.setNum((double)elem->num_val);
            //}
            ans += s1+ " ";
        } else if (elem->type == "operation"){
                ans += elem->operation_val + " ";
        } else if (elem->type == "variable"){
                ans += elem->variable_val + " ";
        }

        que_post.push_elem(elem);
    }
    ui->output_line->setText(ans);
}

Queue MainWindow::get_new_queue(){
    Queue new_que;
    int variables_amount = variables.size();
    int que_post_size = que_post.size();
    for (int i = 0; i < que_post_size; ++i){
        Node * elem = que_post.pop();
        if (elem->type == "variable"){
            QString tmp_variable_name = elem->variable_val;
            for (int i = 0; i < variables_amount; ++i){
                Node * tmp = variables.pop();
                if (tmp->variable_val == tmp_variable_name){
                       new_que.push_num(tmp->num_val);
                    variables.push_elem(tmp);
                    break;
                }
                variables.push_elem(tmp);
            }
        } else {
            new_que.push_elem(elem);
        }
        que_post.push_elem(elem);
    }
    return new_que;
}


void MainWindow::get_variables_from_input(){
    int variables_size = variables.size();
    for(int i = 0; i < variables_size; ++i){
        Node * elem = variables.pop();
            elem->num_val = ui->table_widget->item(i,1)->text().toDouble();
        variables.push_elem(elem);
    }
}


bool MainWindow::validate_comma(QString &str){
    int k1 = 0, k2 = 0;
    for (int i = 0; i < str.length(); ++i){
        if (str[i] == ",") ++k1;
    }
    int que_post_size = que_post.size();
    for (int i = 0; i < que_post_size; ++i){
        Node * elem = que_post.pop();
        if (elem->type == "operation" && elem->operation_val == "log"){
            k2++;
        }
        que_post.push_elem(elem);
    }
    if (k1 != k2){
        return false;
    }
    return true;

}


Queue MainWindow::get_variables_from_queue(){
    int que_post_size = que_post.size();
    Queue ans;
    for (int i = 0 ; i < que_post_size; ++i){
        Node * elem = que_post.pop();
        if (elem->type == "variable"){
            int ans_size = ans.size();
            bool flag = 1;
            for (int i = 0; i < ans_size; ++i){
                Node * tmp = ans.pop();
                if (tmp->variable_val == elem->variable_val){
                    flag = 0;
                    ans.push_elem(tmp);
                    break;
                }
                ans.push_elem(tmp);
            }
            if (flag){
                ans.push("variable", elem->num_val, elem->operation_val,elem->variable_val);
            }
        }
        que_post.push_elem(elem);
    }
    return ans;
}


Queue MainWindow::const_to_num(Queue q){
    int size = q.size();
    for (int i = 0; i < size; ++i){
        Node * elem = q.pop();
        if (elem->type == "variable"){
            if (elem->variable_val == "pi"){
                elem->type = "num";
                elem->num_val = pi;
            } else if (elem->variable_val == "e"){
                elem->type = "num";
                elem->num_val = e;
            }
        }
        q.push_elem(elem);
    }
    return q;
}



bool MainWindow::try_evaluate(Queue q){
    Stack s;
    int sz = q.size();
    for (int i = 0; i < sz; ++i){
        Node * elem = q.pop();
        if (elem->type == "num"){
            s.push_num(elem->num_val);
        } else if (elem->type == "variable"){
            s.push_variable(elem->variable_val);
        }else {
            int arguments_amount = get_argumets_amount(elem->operation_val);
            if (arguments_amount == 2){
                if(s.is_empty()) throw QString("Слишком мало операндов");
                s.pop();
                if(s.is_empty()) throw QString("Слишком мало операндов");
                s.pop();
                double ans = 3;
                s.push_num(ans);
            } else {
                if(s.is_empty()) throw QString("Слишком мало операндов");
                s.pop();
                double ans = 1;
                s.push_num(ans);
            }
        }
        q.push_elem(elem);
    }
    s.pop();
    if (!s.is_empty()) throw QString("Слишком мало операций");
    return true;
}


void MainWindow::validate_space(QString &str){
    int state = 0;
    for (int i = 0; i < str.length(); ++i){
        if ((str[i] == "+" || str[i] == "*" || str[i] == "^" || str[i] == "/" ||
                           str[i] == "-" || str[i] == "," || str[i] == "(" || str[i] == ")" || str[i] == "[" || str[i] == "]")) {
            state = 0;
            continue;
        }
        if (state == 2 && ((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z')|| str[i] == "_" || ('0'<=str[i] && str[i]<='9') || str[i] == ".")) {
            throw QString("Некорректный пробелы");
        }
        if (state == 1 && str[i] == " "){
            state = 2;
            continue;
        }
        if (((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z')|| str[i] == "_" || ('0'<=str[i] && str[i]<='9') || str[i] == ".")) {
            state = 1;
            continue;
        }

    }
}


void MainWindow::on_btn_enter_clicked(){
    try{
        QString str = ui->input_line->text();
        if (str == "") return;
        validate_space(str);
        QString new_str = "";
        for (int i = 0; i < str.length(); ++i){
            if (str[i] != " ") new_str += str[i];
        }
        str = new_str;
        if (str[0] == "+" || str[0] == "/" || str[0] == "*" || str[0] == "^") throw QString("Пропущен операнд");
        if (str[0] == ")" || str[0] == "]") throw QString("Пропущена открывающая скобка");
        if (!checkCBS(str)) throw QString ("Некорректная скобочная последовательность");
        que_post.clear();
        que_in.clear();
        parse(str, 0);
        que_in = const_to_num(que_in);
        que_in.print();
        que_in = validate_function(que_in);
        que_in.print();
        que_post = Dijkstra(que_in);
        que_post.print();
        variables = get_variables_from_queue();
        try_evaluate(que_post);
        output();
        if (!validate_comma(str)) throw QString("Отсутсвтуют необходимые запятые / присутствуют лишние запятые");
        int variables_amount = variables.size();
        ui->lbl_output->clear();
        if (variables_amount == 0){
            double ans = evaluate(get_new_queue());
            write_ans(ans);
        }
        QTableWidget * table = ui->table_widget;
        table->clearContents();
        table->setRowCount(variables_amount);

        QObject :: disconnect(ui->table_widget, SIGNAL(cellChanged(int, int)) , this, SLOT(update_ans(int, int)));
        for (int i = 0; i < variables_amount; ++i){
            Node * elem = variables.pop();
            QTableWidgetItem * item = new QTableWidgetItem;
            item->setText(elem->variable_val);
            item->setFlags(Qt::ItemIsEnabled);
            item->setTextAlignment(Qt::AlignCenter);
            table->setItem(i, 0, item);
            QTableWidgetItem * item_2 = new QTableWidgetItem;
            item_2->setText("");
            item_2->setTextAlignment(Qt::AlignCenter);
            table->setItem(i,1,item_2);
            variables.push("variable",elem->num_val, elem->operation_val,elem->variable_val);
        }
        QObject :: connect(ui->table_widget, SIGNAL(cellChanged(int, int)) , this, SLOT(update_ans(int, int)));

    } catch (QString str){
        QMessageBox::warning(this, "Warning", str);
        que_in.clear();
    }
}


void MainWindow::parse(QString &str, int idx){
     if (idx >= str.length()) return;
     if ((idx == str.length()-1) && (str[idx] == "+" || str[idx] == "*" || str[idx] == "^" || str[idx] == "/" ||
                                     str[idx] == "-" || str[idx] == ",")) throw QString("Отсутствует операнд");
     if (str[idx] == "(" || str[idx] == "["){
         if (str[idx+1] == ")" || str[idx+1] == "]") throw QString("Обнаружены пустые скобки");
         if (idx-1>=0 && ((str[idx-1] >= 'A' && str[idx-1] <= 'Z') || (str[idx-1] >= 'a' && str[idx-1] <= 'z')
                          || str[idx-1] == "_" || ('0'<=str[idx-1] && str[idx-1]<='9')) && que_in.last_in()->type != "operation") throw QString("Отсутствует операнд");
         if (str[idx+1] == "+" || str[idx+1] == "*" || str[idx+1] == "^" || str[idx+1] == "/") throw QString("Отсутствует операнд");
         que_in.push_operation(QString(str[idx]));
         ++idx;
         parse(str, idx);
         return;
     } else if (str[idx] == ")" || str[idx] == "]"){

        if (idx-1>=0 && (str[idx-1] == "+" || str[idx-1] == "*" || str[idx-1] == "^" || str[idx-1] == "/" ||
                         str[idx-1] == "-" || str[idx-1] == ",")) throw QString("Отсутствует операнд");
        if (idx-1 >= 0 && (str[idx-1] == "(" || str[idx-1] == "[")) throw QString("Обнаружены пустые скобки");
        if ((str[idx+1] >= 'A' && str[idx+1] <= 'Z') || (str[idx+1] >= 'a' && str[idx+1] <= 'z')|| str[idx+1] == "_" ||
                ('0'<=str[idx+1] && str[idx+1]<='9')) throw QString("Отсутствует операнд");

        que_in.push_operation(QString(str[idx]));
        ++idx;
        parse(str, idx);
        return;
     } else if ((str[idx] == "-" && idx-1<0) || (str[idx] == "-" && (str[idx-1] == "("||str[idx-1] == "[")) ||
                (str[idx] == "-" && str[idx-1] == ",") || (idx-1>=0 && str[idx-1] == "^" && str[idx] == "-") ||
                (idx-1>=0 && str[idx] == "-" && (str[idx-1] == "(" || str[idx-1] == "[") && (str[idx+1] == ")" || str[idx+1] == "]"))){
         ++idx;
         if (idx >= str.length()) throw QString("Отсутствует операнд");
         que_in.push_operation("~");
         parse(str, idx);
         return;
     } else if (str[idx] == "-" || str[idx] == "+" || str[idx] == "*" || str[idx] == "^" || str[idx] == "/"){
         que_in.push_operation(QString(str[idx]));
         ++idx;
         if ((str[idx] == "-" && idx-1>=0 && str[idx-1] != "^") || str[idx] == "+" || str[idx] == "*" || str[idx] == "^" || str[idx] == "/"){
             throw QString("Отсутствует операнд");
         }
         parse(str, idx);
         return;
     } else if (('0'<=str[idx] && str[idx]<='9') || (str[idx] == ".")){
        Pair tmp_pair = get_num(str, idx);
        idx = tmp_pair.idx;
        if ((str[idx] >= 'A' && str[idx] <= 'Z') || (str[idx] >= 'a' && str[idx] <= 'z')||
                str[idx] == "_") throw QString("Переменная не может начинатся с цифры или отсутствует операция");
        que_in.push_num(tmp_pair.result);
        parse(str, idx);
        return;
     } else if ((str[idx] >= 'A' && str[idx] <= 'Z') || (str[idx] >= 'a' && str[idx] <= 'z') || str[idx] == "_"){
         int d = operation_or_variable(str, idx);
         QString op_or_var = "";
         if (d == 1){
            while ((str[idx] >= 'A' && str[idx] <= 'Z') || (str[idx] >= 'a' && str[idx] <= 'z')){
                op_or_var += str[idx];
                idx++;
            }
            que_in.push_operation(op_or_var);
         } else {
             Pair_string tmp_pair_string = get_var(str, idx);
             idx = tmp_pair_string.idx;
             que_in.push_variable(tmp_pair_string.result);
         }
         parse(str, idx);
         return;
     }else if (str[idx] == ","){
         if (idx-1>=0 && (str[idx-1] == "+" || str[idx-1] == "*" || str[idx-1] == "^" ||
                          str[idx-1] == "/" || str[idx-1] == "-" || str[idx-1] == "," || str[idx+1] == ",")) throw QString("Отсутствует операнд");
         idx++;
         que_in.push_operation(",");
         parse(str, idx);
         return;
     } else {
         throw QString("Присутствуют недопустимые символы");
     }

};



int MainWindow::operation_or_variable(QString &str, int idx){
    QString s = "";
    s+=str[idx];
    if (!(s == "s" || s == "c" || s == "t" || s == "l" || s == "e" || s == "p")) return 2;
    idx++;
    if (!((str[idx] >= 'A' && str[idx] <= 'Z') || (str[idx] >= 'a' && str[idx] <= 'z') || str[idx] == "_" || ('0'<=str[idx] && str[idx]<='9')) && s == "e"){
        return 2;
    } else if (!((str[idx] >= 'A' && str[idx] <= 'Z') || (str[idx] >= 'a' && str[idx] <= 'z')|| str[idx] == "_" || ('0'<=str[idx] && str[idx]<='9')) ){
        return 2;
    } else if (!((str[idx] >= 'A' && str[idx] <= 'Z') || (str[idx] >= 'a' && str[idx] <= 'z')|| str[idx] == "_" || ('0'<=str[idx] && str[idx]<='9'))){
        return 2;
    }
    s+=str[idx];
    if (!(s == "si" || s == "co" || s == "tg" || s == "ct" || s == "lo" || s == "ex" || s == "pi")) return 2;
    idx++;
    if (!((str[idx] >= 'A' && str[idx] <= 'Z') || (str[idx] >= 'a' && str[idx] <= 'z')|| str[idx] == "_" || ('0'<=str[idx] && str[idx]<='9')) && (s == "pi")){
        return 2;
    } else if (!((str[idx] >= 'A' && str[idx] <= 'Z') || (str[idx] >= 'a' && str[idx] <= 'z')|| str[idx] == "_" || ('0'<=str[idx] && str[idx]<='9')) && (s == "tg")){
        return 1;
    } else if (!((str[idx] >= 'A' && str[idx] <= 'Z') || (str[idx] >= 'a' && str[idx] <= 'z')|| str[idx] == "_" || ('0'<=str[idx] && str[idx]<='9'))){
        return 2;
    }
    s+=str[idx];
    if (!(s == "sin" || s == "cos" || s == "ctg" || s == "log" || s == "exp")) return 2;
    idx++;
    if(!((str[idx] >= 'A' && str[idx] <= 'Z') || (str[idx] >= 'a' && str[idx] <= 'z')|| str[idx] == "_"
         || ('0'<=str[idx] && str[idx]<='9')) && (s == "sin" || s == "cos" || s == "ctg" || s == "exp" || s == "log")){
        return 1;
    } else {
        return 2;
    }
}


Pair MainWindow::make_pair(double result, int idx){
    Pair pr;
    pr.result =result;
    pr.idx = idx;
    return pr;
}

Pair MainWindow::get_num(QString &str, int idx){
    double ans;
    QString sz = "", sq = "", se = "";
    if (str[idx] == ".") goto q_part;
    while ('0'<=str[idx] && str[idx]<='9'){
        sz+=str[idx];
        ++idx;
    }
    if (str[idx] == "."){
        sz += ".";
        q_part:
        ++idx;
        while ('0'<=str[idx] && str[idx]<='9'){
            sq+=str[idx];
            ++idx;
        }
        if (sq == "") throw QString("Отсуствует дробная часть числа");
        if (str[idx] == ".") throw QString("Не может быть две дробных части у числа");
    }
    if (str[idx] == "e" || str[idx] == "E"){
        ++idx;
        if (str[idx] == "-"){
            se = "-";
            idx++;
        }
        while ('0'<=str[idx] && str[idx]<='9'){
            se+=str[idx];
            ++idx;
        }
        if (se == "-" || se == "") throw QString("Отсутствует показатель степени");
        if (str[idx] == ".") throw QString("Показатель степени в экспоненциальной записи должен быть целым");
    }
    if (sz =="") sz = "0.";
        ans = (sz+sq).toDouble() * pow_simple(10,se.toInt());
        return make_pair(ans, idx) ;
}



Pair_string MainWindow::get_var(QString &str, int idx){
    Pair_string ans;
    QString s = "";
    while((str[idx] >= 'A' && str[idx] <= 'Z') || (str[idx] >= 'a' && str[idx] <= 'z') || str[idx] == "_" || ('0'<=str[idx] && str[idx]<='9')){
        s += str[idx];
        idx++;
    }
    ans.idx = idx;
    ans.result = s;
    return ans;
}




void MainWindow::update_ans(int row, int col){
    try {
    int variables_amount = 0;
    if (!variables.is_empty()){
         variables_amount = variables.size();
    }
    if (ui->table_widget->rowCount() < variables_amount || (ui->table_widget->rowCount() == 0 && variables_amount == 0)) return;
    for (int i = 0; i < variables_amount; ++i){

            if (!QRegExp("^[-+]?[0-9]*[.,]?[0-9]+(?:[eE][-]?[0-9]+)?$").exactMatch(ui->table_widget->item(i, 1)->text())){
                return;
            }
    }
    on_btn_update_clicked();
    } catch (QString str){
       QMessageBox::warning(this, "Warning", str);
    }
}



QString MainWindow::z_to_s(double a){
    if (equal(a,0)) return QString("0");
    if (equal(a, 1)) return QString ("1");
    if (equal(a,-1)) return QString ("-1");
    if (equal(a, round(a))){
        a = round(a);
    }
    bool flag = 0;
    if (a < 0) {
        flag = 1;
        a = abs(a);
    }
    int k = 0;
    while (a>1){
        k++;
        a /= 10;
    }
    a*=10;
    QString res = "";
    int x = 0;
    while (k){
        --k;
        if (k == 0) a = round(a);
        x = trunc(a);
        a-=trunc(a);
        a*=10;
        res += QString::number(x);
    }

    if (flag) return "-"+res;
    return res;
}


Queue MainWindow::validate_function(Queue q){
    int sz = q.size();
    bool state = 0;
    for (int i = 0; i < sz; ++i){
        Node * elem = q.pop();
        if (state){
            if (elem->operation_val != "(" && elem->operation_val != "[") throw QString("Аргументы функций должны быть заключены в скобки");
            state = 0;
        }
        if (elem->operation_val == "log" || elem->operation_val == "cos" || elem->operation_val == "sin" || elem->operation_val == "exp" ||
                    elem->operation_val == "tg" || elem->operation_val == "ctg"){
                state = 1;
        }

        q.push_elem(elem);
    }
    return q;
}


void MainWindow::write_ans(double ans){
    QString s1;
    if (equal(ans, round(ans))){
        s1 = z_to_s(roundl(ans));
    } else {
        s1.setNum(ans);
    }
    ui->lbl_output->clear();
    ui->lbl_output->setText(s1);
}

void MainWindow::on_btn_update_clicked()
{
    get_variables_from_input();
    Queue new_que = get_new_queue();
    double ans = evaluate(new_que);
    write_ans(ans);
}

void MainWindow::on_btn_angle_clicked(){
    if (angle == 0){
        angle = 1;
        ui->btn_angle->setText("Радианы");
    } else {
        angle = 0;
        ui->btn_angle->setText("Градусы");
    }
}

void MainWindow::on_theme_1_btn_clicked(){ //dark
    pause_flag = 0;
    ui->btn_stop->setText(QString("■"));
    ui->centralwidget->setStyleSheet({"background-image: url(\":/theme_1/back_1.png\");"});
    //backs of rectangles
    ui->input_line->setStyleSheet({"background-image: url(\":/theme_1/simple_back_1.png\"); color : white;"});
    ui->lbl_output->setStyleSheet({"background-image: url(\":/theme_1/simple_back_1.png\"); color : white;"});
   // ui->table_widget->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : white;"});
    ui->output_line->setStyleSheet({"background-image: url(\":/theme_1/simple_back_1.png\"); color : white;"});
    //labels & text_colours
    ui->polis_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : lightGray; "});
    ui->var_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : white;"});
    ui->angle_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : white;"});
    ui->ans_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : lightGray;"});
    ui->theme_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : white;"});
    //buttons
    ui->btn_enter->setStyleSheet({"background-image: url(\":/theme_1/simple_back_1.png\"); color : white;"});
    ui->btn_angle->setStyleSheet({"background-image: url(\":/theme_1/simple_back_1.png\"); color : white;"});
    ui->btn_question->setStyleSheet({"background-image: url(\":/theme_1/simple_back_1.png\"); color : white;"});
    //gif
    ui->btn_stop->show();
    ui->btn_stop->setStyleSheet({"background-image: url(\":/theme_1/simple_back_1.png\"); color : white;"});

    ui->gif_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\");"});
    movieNew = new QMovie(":/theme_1/gif_1.gif" );
    ui->gif_label->setVisible(true);
    ui->gif_label->setMovie(movieNew );
    movieNew -> start();
    ui->table_widget->setStyleSheet("background-image: url('le')");
    auto effect = new QGraphicsDropShadowEffect();
        effect->setBlurRadius(40);
        effect->setOffset(0.5);
        effect->setColor(QColor(51, 153, 255));
    ui->table_widget->setGraphicsEffect(effect);
}

void MainWindow::on_theme_2_btn_clicked(){
    //autumn
    pause_flag = 0;
    ui->btn_stop->setText(QString("■"));
    movieNew ->stop();
    ui->centralwidget->setStyleSheet({"background-image: url(\":/theme_2/back_2.png\");"});
    //backs of rectangles
    ui->input_line->setStyleSheet({"background-image: url(\":/theme_2/simple_back_2.png\"); color : white;"});
    ui->lbl_output->setStyleSheet({"background-image: url(\":/theme_2/simple_back_2.png\"); color : white;"});
   // ui->table_widget->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : white;"});
    ui->output_line->setStyleSheet({"background-image: url(\":/theme_2/simple_back_2.png\"); color : white;"});
    //labels & text_colours
    ui->polis_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : lightGray;"});
    ui->var_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : white;"});
    ui->angle_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : white;"});
    ui->ans_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : lightGray;"});
    ui->theme_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : white;"});
    //buttons
    ui->btn_enter->setStyleSheet({"background-image: url(\":/theme_2/simple_back_2.png\"); color : white;"});
    ui->btn_angle->setStyleSheet({"background-image: url(\":/theme_2/simple_back_2.png\"); color : white;"});
    ui->btn_question->setStyleSheet({"background-image: url(\":/theme_2/simple_back_2.png\"); color : white;"});
    //gif
   // ui->gif_label->setVisible(false);
    ui->gif_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\");"});
    ui->btn_stop->show();
    ui->btn_stop->setStyleSheet({"background-image: url(\":/theme_2/simple_back_2.png\"); color : white;"});
    /*
    QMovie *movieNew = new QMovie(":/theme_1/gif_1.gif" );
    ui->gif_label->setMovie(movieNew );
    movieNew -> start();
    */

    ui->gif_label->show();
    ui->gif_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\");"});
    movieNew->setFileName(":/theme_2/gif_2.gif");
    movieNew -> start();

    ui->table_widget->setStyleSheet("background-image: url('le')");
    auto effect = new QGraphicsDropShadowEffect();
        effect->setBlurRadius(40);
        effect->setOffset(0.5);
        effect->setColor(QColor(255, 204, 0));
    ui->table_widget->setGraphicsEffect(effect);

}

void MainWindow::on_theme_3_btn_clicked()
{
    //clouds
    pause_flag = 0;
    ui->btn_stop->setText(QString("■"));
    movieNew ->stop();
    ui->centralwidget->setStyleSheet({"background-image: url(\":/theme_3/back_3.png\");"});
    //backs of rectangles
    ui->input_line->setStyleSheet({"background-image: url(\":/theme_3/simple_back_3.png\");"});
    ui->lbl_output->setStyleSheet({"background-image: url(\":/theme_3/simple_back_3.png\");"});
    ui->table_widget->setStyleSheet({"background-image: url(\":/other/transp_back.png\");"});
    ui->output_line->setStyleSheet({"background-image: url(\":/theme_3/simple_back_3.png\");"});
    //labels & text_colours
    ui->polis_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : white;"});
    ui->var_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : white;"});
    ui->angle_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : white;"});
    ui->ans_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : white;" });
    ui->theme_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\"); color : white;"});
    //buttons
    ui->btn_enter->setStyleSheet({"background-image: url(\":/theme_3/simple_back_3.png\"); color : white;"});
    ui->btn_angle->setStyleSheet({"background-image: url(\":/theme_3/simple_back_3.png\"); color : white;"});
    ui->btn_question->setStyleSheet({"background-image: url(\":/theme_3/simple_back_3.png\"); color : white;"});
    //gif
    //ui->gif_label->setVisible(false);
    ui->btn_stop->show();
    ui->btn_stop->setStyleSheet({"background-image: url(\":/theme_3/simple_back_3.png\"); color : white;"});
    ui->gif_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\");"});

    ui->gif_label->show();
    ui->gif_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\");"});
    movieNew->setFileName(":/theme_3/gif_3.gif");
    movieNew -> start();

    /*
    QMovie *movieNew = new QMovie(":/theme_1/gif_1.gif" );
    ui->gif_label->setMovie(movieNew );
    movieNew -> start();
    */
    ui->table_widget->setStyleSheet("background-image: url('le')");
    auto effect = new QGraphicsDropShadowEffect();
            effect->setBlurRadius(40);
            effect->setOffset(0.5);
            effect->setColor(QColor(255, 153, 204));
        ui->table_widget->setGraphicsEffect(effect);
}

void MainWindow::on_theme_4_btn_clicked()
{
    //retro
    pause_flag = 0;
    ui->btn_stop->setText(QString("■"));
    movieNew ->stop();
    ui->centralwidget->setStyleSheet({"background-image: url(\":/theme_4/back_4.png\");"});
    //backs of rectangles
    ui->input_line->setStyleSheet({"background-image: url(\":/theme_4/simple_back_4.png\");"});
    ui->lbl_output->setStyleSheet({"background-image: url(\":/theme_4/simple_back_4.png\");"});
   // ui->table_widget->setStyleSheet({"background-image: url(\":/other/transp_back.png\");"});
    ui->output_line->setStyleSheet({"background-image: url(\":/theme_4/simple_back_4.png\");"});
    //labels & text_colours
    ui->polis_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\");"});
    ui->var_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\");"});
    ui->angle_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\");"});
    ui->ans_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\");"});
    ui->theme_label->setStyleSheet({"background-image: url(\":/other/transp_back.png\");"});
    //buttons
    ui->btn_enter->setStyleSheet({"background-image: url(\":/theme_4/simple_back_4.png\");"});
    ui->btn_angle->setStyleSheet({"background-image: url(\":/theme_4/simple_back_4.png\");"});
    ui->btn_question->setStyleSheet({"background-image: url(\":/theme_4/simple_back_4.png\");"});
    ui->btn_stop->hide();
    //gif
    ui->gif_label->hide();
    ui->table_widget->setStyleSheet("background-image: url('le')");
    auto effect = new QGraphicsDropShadowEffect();
            effect->setBlurRadius(40);
            effect->setOffset(0.5);
            effect->setColor(QColor(255, 217, 179));
        ui->table_widget->setGraphicsEffect(effect);
}





void question_button_clicked(QObject * obj) {
    if(question_button_flag == 0) {
        QPixmap a(":/question/question_2.png");
        question_label->setPixmap(a);
        question_widget->setStyleSheet("background-image: url(\":/theme_2/back_2.png\")");
        question_button->setStyleSheet("background-image: url(\":/theme_2/back_2.png\"); color : white;");
        question_button_flag++;
    }
    else if (question_button_flag == 1) {
        QPixmap a(":/question/question_3.png");
        question_label->setPixmap(a);
        question_widget->setStyleSheet("background-image: url(\":/theme_3/simple_back_3.png\")");
        question_button->setStyleSheet("background-image: url(\":/theme_3/simple_back_3.png\"); color : black;");
        question_button_flag++;
        question_button->setText("Спасибо");
    }
    else {
        question_button_flag = 0;
        question_widget->hide();
    }
}






void MainWindow::on_btn_question_clicked(){
    question_widget->hide();
    QPixmap a(":/question/question_1.png");
    question_label->setPixmap(a);
    question_button->setText("Далее");
    question_widget->setStyleSheet("background-image: url(\":/theme_1/simple_back_1.png\")");
    question_button->setStyleSheet("background-image: url(\":/theme_1/simple_back_1.png\"); color : white;");
    question_button_flag = 0;
    question_widget->show();
}

void MainWindow::on_btn_stop_clicked(){
    if (pause_flag == 0) {
        ui->btn_stop->setText(QString("▶"));
        pause_flag = 1;
        movieNew -> stop();
    }
    else {
        pause_flag = 0;
        ui->btn_stop->setText(QString("■"));
        movieNew -> start();
    }

}
