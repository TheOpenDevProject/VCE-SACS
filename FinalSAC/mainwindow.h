#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QStringList>
#include <vector>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void updateVotePercentages(QModelIndex &index) const;
    void updateTotal();
    void updateModelBreakDowns();
private slots:
    void on_actionPopulate_Sample_Data_triggered();
    void onPlayerInformationChanged(QModelIndex index,QModelIndex index_side);
    void onMatchOneSheetChanged(QModelIndex index,QModelIndex index_side);
    void onMatchTwoSheetChanged(QModelIndex index, QModelIndex index_side);
    void onMatchThreeSheetChanged(QModelIndex index, QModelIndex index_side);
    void on_actionForce_Recalculate_triggered();

    void on_actionNew_Match_Sheet_triggered();

private:
    Ui::MainWindow *ui;
    QStringList PlayerInformationHeaders;
    QStandardItemModel *PlayerInformationModel = new QStandardItemModel(this);
    ///////////////////////////////////////////////////////////////////////////
    QStringList matchSheetHeaders_;
    std::vector<QStandardItemModel*> matchSheets_;

};

#endif // MAINWINDOW_H
