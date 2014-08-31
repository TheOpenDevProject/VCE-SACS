#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <algorithm>
#ifndef TRUE
#define TRUE true
#endif

#ifndef FALSE
#define FALSE false
#endif

#define RANGE_MAX 9
//Note: Tables are linked through their models and populate eachothers data sets as themselves change
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Player Table
    PlayerInformationHeaders << "Player First" << "Player Last" << "Player Of The Season" << "Goals Paying" << "Vote Count " << "Vote Count (%)" << "Vote Bonus ($)" << "Total Owing ($)";
    PlayerInformationModel->setRowCount(1);
    PlayerInformationModel->setColumnCount(8);
    PlayerInformationModel->setHorizontalHeaderLabels(PlayerInformationHeaders);
   ui->playerInfoTable->setModel(PlayerInformationModel);

   //Player Table
//Data Change Monitor Signals
   connect(PlayerInformationModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(onPlayerInformationChanged(QModelIndex,QModelIndex)));

   //Build a base structure for our match sheets
   matchSheets_.push_back(new QStandardItemModel());
    matchSheets_.push_back(new QStandardItemModel());
     matchSheets_.push_back(new QStandardItemModel());

     connect(matchSheets_.at(0),SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(onMatchOneSheetChanged(QModelIndex,QModelIndex)));
        connect(matchSheets_.at(1),SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(onMatchTwoSheetChanged(QModelIndex,QModelIndex)));
            connect(matchSheets_.at(2),SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(onMatchThreeSheetChanged(QModelIndex,QModelIndex)));

     //Yep that just happened... You're not reading this wrong, I called a slot as a function +10 design boss mode 9000
     on_actionPopulate_Sample_Data_triggered();

   matchSheetHeaders_ << "Player First" << "Player Last" << "Player Scored (True/False)" << "Votes For Match" << "Match Outcome";

   for(int i = 0; i < matchSheets_.size(); i++){
       matchSheets_.at(i)->setRowCount(16); //Welcome to hacksville, I just kinda sorta know there are going to be 16 spots on each team sheet, sweg only the first 11 get payed though bcz the rest are baddies
       matchSheets_.at(i)->setColumnCount(5);
       matchSheets_.at(i)->setHorizontalHeaderLabels(matchSheetHeaders_);
       //Alright everyone sit down its time for a super hack WHOOOOOOSH
       for(int k = 0; k < PlayerInformationModel->rowCount(); k++){
           //We are going to take the players out of the first model and put them into each match sheet, this function can be called to re-sync the models
           matchSheets_.at(i)->setData(matchSheets_.at(i)->index(k,0),PlayerInformationModel->index(k,0).data());
           matchSheets_.at(i)->setData(matchSheets_.at(i)->index(k,1),PlayerInformationModel->index(k,1).data());

       }
   }

   //If this was real this would be terribad code but since my application is not generic enough who even cares...
   ui->matchOneTable->setModel(matchSheets_.at(0));
   ui->matchTwoTable->setModel(matchSheets_.at(1));
   ui->matchThreeTable->setModel(matchSheets_.at(2));
}

MainWindow::~MainWindow()
{

    delete ui;
    //Cleanup the match sheets
    for(int i = 0; i < matchSheets_.size(); i++){
       delete matchSheets_.at(i);
       qDebug() << "Match Sheet " << i << "removed from FSTORE";
    }
}

void MainWindow::updateVotePercentages(QModelIndex &index) const
{
//Nukes ahoy steady as she goes
    qDebug() << PlayerInformationModel->index(index.row(),index.column()).data().toInt();

     if(PlayerInformationModel->index(index.row(),index.column()).data().toInt() > 9){
        QMessageBox warn;
        warn.setText("Must be less than or equal to 9");
        warn.exec();
        //Set at allowed Max (See const define of RANGE_MAX for details or the back of the box or something I dunno...)
        PlayerInformationModel->setData(PlayerInformationModel->index(index.row(),index.column()),RANGE_MAX);

    }else{
        //Display our data in the current column offset (1)
    PlayerInformationModel->setData(PlayerInformationModel->index(index.row(),index.column() + 1),(float)(PlayerInformationModel->index(index.row(),index.column()).data().toInt() * 0.1));
     }
}

void MainWindow::updateTotal()
{
    int playerOfSeasonBonus = 0;
    //This is my version of a syncronous update of ALL players total owings at once, Just call this function to recalculate at any point in the program

    //Probably safer to use foreach(C++11) but this projects code must be kept as readable as possible (Had a marking scheme)
    for(int i = 0;i < PlayerInformationModel->rowCount(); i++){
        //Check for player of the season bonus
        if(PlayerInformationModel->index(i,2).data().toBool() == TRUE){
            playerOfSeasonBonus = 1000;
        }else{
            //Reset it
            playerOfSeasonBonus = 0;
        }
       unsigned int playerTotal = 0; //Initialise at 0 to be safe
        //Just created the expression 100x - 50
       //This will probably be invalid due to a bug I included :) - The bug is that Goals scored is not calculated properly
       playerTotal = (750) + playerOfSeasonBonus + (100 * PlayerInformationModel->index(i,4).data().toInt() - 50) + (PlayerInformationModel->index(i,3).data().toInt() * 30);
       qDebug() << playerTotal;
    PlayerInformationModel->setData(PlayerInformationModel->index(i,7),playerTotal);
    }
}

void MainWindow::on_actionPopulate_Sample_Data_triggered()
{
    PlayerInformationModel->setRowCount(16);
    PlayerInformationModel->setData(PlayerInformationModel->index(0,0),"Anna");
    PlayerInformationModel->setData(PlayerInformationModel->index(0,1),"Belle");
    PlayerInformationModel->setData(PlayerInformationModel->index(0,2),TRUE);
    PlayerInformationModel->setData(PlayerInformationModel->index(0,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(0,4),9);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(0,5),(float)(PlayerInformationModel->index(0,4).data().toInt() * 0.1));

    //Player 2
    PlayerInformationModel->setData(PlayerInformationModel->index(1,0),"Betty");
    PlayerInformationModel->setData(PlayerInformationModel->index(1,1),"Chandler");
    PlayerInformationModel->setData(PlayerInformationModel->index(1,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(1,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(1,4),2);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(1,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));


    //Player 3
    PlayerInformationModel->setData(PlayerInformationModel->index(2,0),"Cathy");
    PlayerInformationModel->setData(PlayerInformationModel->index(2,1),"Dyson");
    PlayerInformationModel->setData(PlayerInformationModel->index(2,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(2,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(2,4),2);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(2,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));




    //Player 4
    PlayerInformationModel->setData(PlayerInformationModel->index(3,0),"Donna");
    PlayerInformationModel->setData(PlayerInformationModel->index(3,1),"Edison");
    PlayerInformationModel->setData(PlayerInformationModel->index(3,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(3,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(3,4),4);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(3,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));





    //Player 5
    PlayerInformationModel->setData(PlayerInformationModel->index(4,0),"Emily");
    PlayerInformationModel->setData(PlayerInformationModel->index(4,1),"Farrell");
    PlayerInformationModel->setData(PlayerInformationModel->index(4,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(4,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(4,4),4);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(4,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));





    //Player 6
    PlayerInformationModel->setData(PlayerInformationModel->index(5,0),"Fiona");
    //Onyah Garrett
    PlayerInformationModel->setData(PlayerInformationModel->index(5,1),"Garrett");
    PlayerInformationModel->setData(PlayerInformationModel->index(5,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(5,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(5,4),4);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(5,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));





    //Player 7
    PlayerInformationModel->setData(PlayerInformationModel->index(6,0),"Gina");
    PlayerInformationModel->setData(PlayerInformationModel->index(6,1),"Harper");
    PlayerInformationModel->setData(PlayerInformationModel->index(6,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(6,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(6,4),4);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(6,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));





    //Player 8
    PlayerInformationModel->setData(PlayerInformationModel->index(7,0),"Henrietta");
    PlayerInformationModel->setData(PlayerInformationModel->index(7,1),"Jones");
    PlayerInformationModel->setData(PlayerInformationModel->index(7,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(7,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(7,4),4);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(7,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));




    //Player 9
    PlayerInformationModel->setData(PlayerInformationModel->index(8,0),"Jenny");
    PlayerInformationModel->setData(PlayerInformationModel->index(8,1),"Kennedy");
    PlayerInformationModel->setData(PlayerInformationModel->index(8,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(8,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(8,4),4);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(8,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));





    //Player 10
    PlayerInformationModel->setData(PlayerInformationModel->index(9,0),"Kathy");
    PlayerInformationModel->setData(PlayerInformationModel->index(9,1),"Louis");
    PlayerInformationModel->setData(PlayerInformationModel->index(9,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(9,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(9,4),4);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(9,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));





    //Player 11
    PlayerInformationModel->setData(PlayerInformationModel->index(10,0),"Lisa");
    PlayerInformationModel->setData(PlayerInformationModel->index(10,1),"Madison");
    PlayerInformationModel->setData(PlayerInformationModel->index(10,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(10,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(10,4),4);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(10,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));





    //Player 12
    PlayerInformationModel->setData(PlayerInformationModel->index(11,0),"Mary");
    PlayerInformationModel->setData(PlayerInformationModel->index(11,1),"Nova");
    PlayerInformationModel->setData(PlayerInformationModel->index(11,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(11,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(11,4),4);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(11,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));




    //Player 13
    PlayerInformationModel->setData(PlayerInformationModel->index(12,0),"Nina");
    PlayerInformationModel->setData(PlayerInformationModel->index(12,1),"Porter");
    PlayerInformationModel->setData(PlayerInformationModel->index(12,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(12,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(12,4),4);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(12,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));




    //Player 14
    PlayerInformationModel->setData(PlayerInformationModel->index(13,0),"Tina");
    PlayerInformationModel->setData(PlayerInformationModel->index(13,1),"Vella");
    PlayerInformationModel->setData(PlayerInformationModel->index(13,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(13,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(13,4),4);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(13,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));




    //Player 15
    PlayerInformationModel->setData(PlayerInformationModel->index(14,0),"Rosa");
    PlayerInformationModel->setData(PlayerInformationModel->index(14,1),"Sanders");
    PlayerInformationModel->setData(PlayerInformationModel->index(14,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(14,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(14,4),4);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(14,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));




    //Player 16
    PlayerInformationModel->setData(PlayerInformationModel->index(15,0),"Wendy");
    PlayerInformationModel->setData(PlayerInformationModel->index(15,1),"Anderson");
    PlayerInformationModel->setData(PlayerInformationModel->index(15,2),FALSE);
    PlayerInformationModel->setData(PlayerInformationModel->index(15,3),0);
    PlayerInformationModel->setData(PlayerInformationModel->index(15,4),4);
    //This is just a little percentage hack using C-Style casting and then times by .1 to get the percentage, I explicitly cast because QVariant might decide to use an Int
    //which would break my later calculations
    PlayerInformationModel->setData(PlayerInformationModel->index(15,5),(float)(PlayerInformationModel->index(1,4).data().toInt() * 0.1));

    //Fill Match Sheets With Sample Data

}

void MainWindow::onPlayerInformationChanged(QModelIndex index, QModelIndex index_side)
{
 //Side note for the Qt-Project, This signal's signature should be changed to reflect the QStandardItemModels 2D RC struct (Maybe I missread the docs and its meant to be 4D?)
//Oh well anyway enough booting about we only need to use index because both parameters contain the same value for us.
    qDebug() << index.row() << index.column();

    //If this is changed we need to recalculate the percentages
    if(index.column() == 4){
    updateVotePercentages(index);
    }

 //Can be thought of as a kind of GL render, Should always be included when the data has been changed.
    updateTotal();
}

void MainWindow::onMatchOneSheetChanged(QModelIndex index, QModelIndex index_side)
{
    //Votes
  if(index.column() == 3 ){
       int voteTotal = matchSheets_.at(0)->index(index.row(),3).data().toInt() + matchSheets_.at(1)->index(index.row(),3).data().toInt() + matchSheets_.at(2)->index(index.row(),3).data().toInt();
      PlayerInformationModel->setData(PlayerInformationModel->index(index.row(),4),voteTotal);
  }
  //Goals (Note: Has an intentional bug, this was so I would have something to write about for the VCAA :)
  if(index.column() == 2){
      if(matchSheets_.at(0)->index(index.row(),2).data().toBool() == TRUE){
          PlayerInformationModel->setData(PlayerInformationModel->index(index.row(),3),PlayerInformationModel->index(index.row(),3).data().toInt() + 1);
      }else{
             PlayerInformationModel->setData(PlayerInformationModel->index(index.row(),3),PlayerInformationModel->index(index.row(),3).data().toInt() - 1);
      }
  }

}

void MainWindow::onMatchTwoSheetChanged(QModelIndex index, QModelIndex index_side)
{
    if(index.column() == 3 ){
         int voteTotal = matchSheets_.at(0)->index(index.row(),3).data().toInt() + matchSheets_.at(1)->index(index.row(),3).data().toInt() + matchSheets_.at(2)->index(index.row(),3).data().toInt();
       PlayerInformationModel->setData(PlayerInformationModel->index(index.row(),4),voteTotal);
    }

    //Goals (Note: Has an intentional bug, this was so I would have something to write about for the VCAA :)
    if(index.column() == 2){
        if(matchSheets_.at(0)->index(index.row(),2).data().toBool() == TRUE){
            PlayerInformationModel->setData(PlayerInformationModel->index(index.row(),3),PlayerInformationModel->index(index.row(),3).data().toInt() + 1);
        }else{
               PlayerInformationModel->setData(PlayerInformationModel->index(index.row(),3),PlayerInformationModel->index(index.row(),3).data().toInt() - 1);
        }
    }
}

void MainWindow::onMatchThreeSheetChanged(QModelIndex index, QModelIndex index_side)
{
    if(index.column() == 3 ){
         int voteTotal = matchSheets_.at(0)->index(index.row(),3).data().toInt() + matchSheets_.at(1)->index(index.row(),3).data().toInt() + matchSheets_.at(2)->index(index.row(),3).data().toInt();
        PlayerInformationModel->setData(PlayerInformationModel->index(index.row(),4),voteTotal);
    }

    //Goals (Note: Has an intentional bug, this was so I would have something to write about for the VCAA :)
    if(index.column() == 2){
        if(matchSheets_.at(0)->index(index.row(),2).data().toBool() == TRUE){
            PlayerInformationModel->setData(PlayerInformationModel->index(index.row(),3),PlayerInformationModel->index(index.row(),3).data().toInt() + 1);
        }else{
               PlayerInformationModel->setData(PlayerInformationModel->index(index.row(),3),PlayerInformationModel->index(index.row(),3).data().toInt() - 1);
        }
    }
}

void MainWindow::on_actionForce_Recalculate_triggered()
{
    updateTotal();
}

void MainWindow::on_actionNew_Match_Sheet_triggered()
{


}
