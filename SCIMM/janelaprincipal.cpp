#include "janelaprincipal.h"
#include "ui_janelaprincipal.h"

#include "qcustomplot.h"
#include <iostream>
#include <unistd.h>
#include <string>
#include <qdebug.h>
#include <QCloseEvent>
#include "calibracao.h"
#include "automatica.h"
#include "manual.h"
#include <stdio.h>
#include <stdlib.h>
Automatica CA;
Manual CM;
QComboBox *op;
bool INDISPONIVEL;
int CAMERA =1;
JanelaPrincipal::JanelaPrincipal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::JanelaPrincipal)
{
setWindowIcon(QIcon(":/icons/icone.png"));


    ui->setupUi(this);
    //Inicialização de Variaveis
    cor.COR_INDICE = 0;
    MIN[0] = MIN[1]  = MIN[2] = 0;
    MAX[0] = MAX[1]  = MAX[2] =256;
    INDICE_CALIBRACAO =  INDEX = 0;
    ui->EDIT_CM_MAX->setText("0");
    ui->EDIT_CM_MAX->setText("256");
    ui->SLIDER_CM_MIN->setValue(0);
    ui->SLIDER_CM_MAX->setValue(256);
    memset(CALIBRADO, 0, sizeof(CALIBRADO));






    //Ações Menu
    connect(ui->actionAutomatica, SIGNAL(triggered(bool)), SLOT(MenuAutomatico()));
    connect(ui->actionManual, SIGNAL(triggered(bool)), SLOT(MenuManual()));
    connect(ui->actionSobre, SIGNAL(triggered(bool)), SLOT(MenuSobre()));


    //Ações Calibração Manual
    connect(ui->CB_CM_CORES, SIGNAL(currentIndexChanged(int)), SLOT(ComboChanged(int)));
    connect(ui->TAB_HSV, SIGNAL(currentChanged(int)), SLOT(TabChangedTo(int)));
    connect(ui->SLIDER_CM_MAX, SIGNAL(valueChanged(int)), SLOT(setMaxTextInt(int)));
    connect(ui->EDIT_CM_MAX, SIGNAL(textChanged(QString)), SLOT(setMaxSliderValue()));
    connect(ui->SLIDER_CM_MIN, SIGNAL(valueChanged(int)), SLOT(setMinTextInt(int)));
    connect(ui->EDIT_CM_MIN, SIGNAL(textChanged(QString)), SLOT(setMinSliderValue()));
    connect(ui->BT_CM_CALIBRAR, SIGNAL(clicked(bool)), SLOT(IniciarCalibracao()));
    connect(ui->BT_CM_SALVAR, SIGNAL(clicked(bool)), SLOT(SalvarCalibracao()));
    connect(ui->BT_CM_FINALIZAR, SIGNAL(clicked(bool)), SLOT(FinalizarCalibracao()));
    connect(ui->BT_CM_INICIAR, SIGNAL(clicked(bool)), SLOT(IniciarCameraManual()));
    //Ações Calibração Automatica
    connect(ui->BT_CA_INICIAR, SIGNAL(clicked(bool)), SLOT(BotaoIniciar()));
    connect(ui->BT_CA_SALVAR, SIGNAL(clicked(bool)), SLOT(BotaoSalvar()));
    connect(ui->BT_CA_FINALIZAR, SIGNAL(clicked(bool)), SLOT(BotaoFinalizar()));
    connect(ui->CB_CA_CORES, SIGNAL(currentIndexChanged(int)), SLOT(ComboCorChanged(int)));
    connect(ui->BT_CA_INICIAR_CAM, SIGNAL(clicked(bool)), SLOT(IniciarCameraAutomatico()));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), SLOT(ComboObjetoChanged(int)));

}

void JanelaPrincipal::CameraIndisponivel(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, "SCIMM", "Camera Indisponivel");
    INDISPONIVEL = true;
}

// ----------- Métodos Menu -----------
void JanelaPrincipal::MenuManual(){
    CA.Fechar();
    ResetarTelas();
    ui->TELAS->setCurrentIndex(0);


}
void JanelaPrincipal::MenuAutomatico(){
   CM.Fechar();
    ResetarTelas();
    ui->TELAS->setCurrentIndex(1);

}
void JanelaPrincipal::MenuSobre(){
    ResetarTelas();
    ui->TELAS->setCurrentIndex(2);
}
void JanelaPrincipal::ResetarTelas(){
    ui->BT_CA_INICIAR_CAM->setEnabled(true);
    ui->BT_CM_INICIAR->setEnabled(true);

    ui->BT_CM_CALIBRAR->setEnabled(false);
    ui->BT_CM_SALVAR->setEnabled(false);
    ui->BT_CM_FINALIZAR->setEnabled(false);
    ui->CB_CM_CORES->setEnabled(false);
    ui->SLIDER_CM_MAX->setEnabled(false);
    ui->SLIDER_CM_MIN->setEnabled(false);
    ui->EDIT_CM_MAX->setEnabled(false);
    ui->EDIT_CM_MIN->setEnabled(false);
    ui->BT_CA_INICIAR->setEnabled(false);
    ui->BT_CA_SALVAR->setEnabled(false);
    ui->BT_CA_FINALIZAR->setEnabled(false);
    ui->CB_CA_CORES->setEnabled(false);
    ui->comboBox->setEnabled(false);
    ui->PROGRESS_CA->setEnabled(false);
    INDISPONIVEL= false;

}

// ----------- Métodos Calibração Manual -----------
void JanelaPrincipal::ComboChanged(int index){
    INDICE_CALIBRACAO = index;
    ui->CHECK_CM_CALIBRADO->setChecked(CALIBRADO[INDICE_CALIBRACAO]);


}
void JanelaPrincipal::SetHSV(int* h, int* s, int* v){
    H = h;
    S = s;
    V = v;
    MAX[0] = MAX[1] = MAX[2] =256;
    MIN[0] =MIN[1]=MIN[2]=0;
    Plot(H,S,V);
}
void JanelaPrincipal::IniciarCalibracao(){

    CALIBRAR = true;
}
void JanelaPrincipal::SalvarCalibracao(){
    CALIBRADO[INDICE_CALIBRACAO] =  true;
    ui->CHECK_CM_CALIBRADO->setChecked(CALIBRADO[INDICE_CALIBRACAO]);

    cor.COR_INDICE = INDICE_CALIBRACAO;
    cor.SetMax(MAX);
    cor.SetMin(MIN);

}
void JanelaPrincipal::FinalizarCalibracao(){
    // std::cout << "Finalizada " << std::endl;
    CM.Fechar();
    FINALIZADA = true;
      this->close();
}
void JanelaPrincipal::setMaxTextInt(int value){

    ui->EDIT_CM_MAX->setText(QString::number(value));
    MAX[INDEX] = value;
    switch (INDEX) {
    case 0:
        RePlot(H);
        break;
    case 1:
        RePlot(S);
        break;
    case 2:
        RePlot(V);
        break;
    default:
        break;

    }
}
void JanelaPrincipal::setMaxSliderValue(){
    ui->SLIDER_CM_MAX->setValue(ui->EDIT_CM_MAX->text().toInt());
    MAX[INDEX] = ui->EDIT_CM_MAX->text().toInt();

    switch (INDEX) {
    case 0:
        RePlot(H);
        break;
    case 1:
        RePlot(S);
        break;
    case 2:
        RePlot(V);
        break;
    default:
        break;
    }
}
void JanelaPrincipal::setMinTextInt(int value){
    ui->EDIT_CM_MIN->setText(QString::number(value));
    MIN[INDEX] = value;

    switch (INDEX) {
    case 0:
        RePlot(H);
        break;
    case 1:
        RePlot(S);
        break;
    case 2:
        RePlot(V);
        break;
    default:
        break;
    }
}
void JanelaPrincipal::setMinSliderValue(){
    ui->SLIDER_CM_MIN->setValue(ui->EDIT_CM_MIN->text().toInt());
    MIN[INDEX] = ui->EDIT_CM_MIN->text().toInt();
    switch (INDEX) {
    case 0:
        RePlot(H);
        break;
    case 1:
        RePlot(S);
        break;
    case 2:
        RePlot(V);
        break;
    default:
        break;
    }
}
void JanelaPrincipal::TabChangedTo(int index){
    INDEX = index;
    ui->SLIDER_CM_MIN->setValue(MIN[INDEX]);
    ui->SLIDER_CM_MAX->setValue(MAX[INDEX]);

    switch (INDEX) {
    case 0:
        RePlot(H);
        break;
    case 1:
        RePlot(S);
        break;
    case 2:
        RePlot(V);
        break;
    default:
        break;
    }

}
void JanelaPrincipal::Plot(int *h , int *s, int *v){
    /* obter min */
    int i;
    /*for (i=0; i<256; ++i)
    {
        if(h[i]!=0){
            MIN[0] = i;
            break;
        }
    }*/

    h[256]=1;
    for (i=0; h[i]!=0; ++i);
    MIN[0] = i==256?0:1;

    ui->SLIDER_CM_MIN->setValue(MIN[0]);

    s[256]=1;
    for (i=0; s[i]!=0; ++i);
    MIN[1] = i==256?0:i;

    v[256]=1;
    for (i=0; v[i]!=0; ++i);
    MIN[2] = i==256?0:i;


    /*  for (i=0; i<256; ++i)
    {
        if(v[i]!=0){
            MIN[2] = i;
            break;
        }
    }


    for (i= MIN[2]; i<256; ++i)
    {
        if(v[i]==0){
            MAX[2] = i-1;
            break;
        }
    }
*/

    /* ------------------------- H -------------------------*/
    QVector<double> xH(256), yH(4000);
    for (int i=MIN[0]; i<256; ++i)
    {       if(h[i] > 0){
            xH[i] = i;
            if(h[i] > 1000)
                yH[i] =1000;
            else
                yH[i]=h[i];      }     }
    ui->customPlotH->addGraph();
    ui-> customPlotH->graph(0)->setData(xH, yH);
    ui->customPlotH->xAxis->setLabel("H");
    ui-> customPlotH->yAxis->setLabel("Qtd");
    ui-> customPlotH->xAxis->setRange(MIN[0], 256);
    ui-> customPlotH->yAxis->setRange(0, 1000);
    ui-> customPlotH->replot();

    /* ------------------------- S -------------------------*/
    QVector<double> xS(256), yS(4000);
    for (int i=MIN[1]; i<256; ++i)
    {       if(s[i] > 0){
            xS[i] = i;
            if(s[i] > 1000)
                yS[i] =1000;
            else
                yS[i]=s[i];      }     }
    ui->customPlotS->addGraph();
    ui-> customPlotS->graph(0)->setData(xS, yS);
    ui->customPlotS->xAxis->setLabel("S");
    ui-> customPlotS->yAxis->setLabel("Qtd");
    ui-> customPlotS->xAxis->setRange(MIN[1], 256);
    ui-> customPlotS->yAxis->setRange(0, 1000);
    ui-> customPlotS->replot();

    /* ------------------------- v -------------------------*/
    QVector<double> xV(256), yV(4000);
    for (int i=MIN[2]; i<256; ++i)
    {       if(v[i] > 0){
            xV[i] = i;
            if(v[i] > 1000)
                yV[i] =1000;
            else
                yV[i]=v[i];      }     }
    ui->customPlotV->addGraph();
    ui-> customPlotV->graph(0)->setData(xV, yV);
    ui->customPlotV->xAxis->setLabel("S");
    ui-> customPlotV->yAxis->setLabel("Qtd");
    ui-> customPlotV->xAxis->setRange(MIN[2], 256);
    ui-> customPlotV->yAxis->setRange(0, 1000);
    ui-> customPlotV->replot();
}
void JanelaPrincipal::RePlot(int a[256]){
    QVector<double> x(MAX[INDEX]), y(4000);
    for (int i=MIN[INDEX]; i<MAX[INDEX]; ++i)
    {       if(a[i] > 0){
            x[i] = i;
            if(a[i] > 1000)
                y[i] =1000;
            else
                y[i]=a[i];      }     }
    switch (INDEX) {
    case 0:
        ui->customPlotH->addGraph();
        ui-> customPlotH->graph(0)->setData(x, y);
        ui->customPlotH->xAxis->setLabel("H");
        ui-> customPlotH->yAxis->setLabel("Qtd");
        ui-> customPlotH->xAxis->setRange(MIN[INDEX], MAX[INDEX]);
        ui-> customPlotH->yAxis->setRange(0, 1000);
        ui-> customPlotH->replot();
        break;
    case 1:
        ui->customPlotS->addGraph();
        ui-> customPlotS->graph(0)->setData(x, y);
        ui->customPlotS->xAxis->setLabel("S");
        ui-> customPlotS->yAxis->setLabel("Qtd");
        ui-> customPlotS->xAxis->setRange(MIN[INDEX], MAX[INDEX]);
        ui-> customPlotS->yAxis->setRange(0, 1000);
        ui-> customPlotS->replot();
        break;
    case 2:
        ui->customPlotV->addGraph();
        ui-> customPlotV->graph(0)->setData(x, y);
        ui->customPlotV->xAxis->setLabel("V");
        ui-> customPlotV->yAxis->setLabel("Qtd");
        ui-> customPlotV->xAxis->setRange(MIN[INDEX], MAX[INDEX]);
        ui-> customPlotV->yAxis->setRange(0, 1000);
        ui-> customPlotV->replot();
        break;
    default:
        break;
    }

}
void JanelaPrincipal::IniciarCameraManual(){
     ui->BT_CM_INICIAR->setEnabled(false);
     ui->BT_CM_CALIBRAR->setEnabled(true);
     ui->BT_CM_SALVAR->setEnabled(true);
     ui->BT_CM_FINALIZAR->setEnabled(true);
     ui->SLIDER_CM_MAX->setEnabled(true);
     ui->SLIDER_CM_MIN->setEnabled(true);
     ui->EDIT_CM_MAX->setEnabled(true);
     ui->EDIT_CM_MIN->setEnabled(true);
    ui-> CB_CM_CORES->setEnabled(true);
    CM.Iniciar(this, CAMERA);

    if(INDISPONIVEL) {
        ResetarTelas();
    }


    //CM.Iniciar(this, CAMERA);
}

// ----------- Métodos Calibração Automatico -----------

void JanelaPrincipal::SetText(int qtd){

   for(int i=0;i<qtd;i++){
         ui->comboBox->addItem(QString::number(i));
  }

}
void JanelaPrincipal::BotaoSalvar(){
    SALVAR=true;
    INDICE_COR = ui->CB_CA_CORES->currentIndex();
    INDICE_OBJETO = ui->comboBox->currentIndex();
    CALIBRADO[INDICE_COR] = true;
    ui->CHECK_CA_CALIBRADO->setChecked( CALIBRADO[INDICE_COR]);
}
void JanelaPrincipal::BotaoIniciar(){
    INICIAR = true;
    ui->BT_CA_INICIAR->setEnabled(false);
}
void JanelaPrincipal::SetStatus(int porcento, std::string mensagem){

    ui->LABEL_CA_STATUS->setText(QString::fromStdString(mensagem));
    ui->comboBox->repaint();
    ui->PROGRESS_CA->setValue(porcento);
    ui->PROGRESS_CA->repaint();
    if(porcento>90){
        ui->CB_CA_CORES->setEnabled(true);
    }

}
void JanelaPrincipal::BotaoFinalizar(){
 CA.Fechar();
    FINALIZADA = true;
    this->close();

}
void JanelaPrincipal::ComboObjetoChanged(int index){
    INDICE_OBJETO = index;
    ui->CHECK_CA_CALIBRADO->setChecked(CALIBRADO[INDICE_COR]);
}
void JanelaPrincipal::ComboCorChanged(int index){
    INDICE_COR = index;
    ui->CHECK_CA_CALIBRADO->setChecked(CALIBRADO[INDICE_COR]);
}
void JanelaPrincipal::IniciarCameraAutomatico(){
    ui->BT_CA_INICIAR_CAM->setEnabled(false);
    ui->BT_CA_INICIAR->setEnabled(true);
    ui->BT_CA_SALVAR->setEnabled(true);
    ui->BT_CA_FINALIZAR->setEnabled(true);
    ui->CB_CA_CORES->setEnabled(true);
    ui->comboBox->setEnabled(true);
    ui->PROGRESS_CA->setEnabled(true);
    CA.Iniciar(this, CAMERA);
    if(INDISPONIVEL) {
        ResetarTelas();
    }

}


JanelaPrincipal::~JanelaPrincipal()
{
    delete ui;
}
