#include "cmdfacade.h"

CmdFacade::CmdFacade(QObject *parent) : QObject(parent)
{

}

const s_account* CmdFacade::getAccountByID(int ID){
    for(auto& account : m_getAccounts){
        if(account.AccID == ID){
            return &account;
        }
    }
    return nullptr;
}
