#include "passwd-helper.h"

#include <crypt.h>
#include <errno.h>
#include <error.h>
#include <security/pam_appl.h>
#include <QByteArray>
#include <QDebug>
#include <QString>
#include <random>
#include <qt5-log-i.h>

bool PasswdHelper::encryptPassword(const QString &pwd, QString &encrypted)
{
    QByteArray byteArray = pwd.toLatin1();
    QString saltChar = "ABCDEFGHIJKLMNOPQRSTUVXYZabcdefghijklmnopqrstuvxyz./0123456789";

    QString rand16SaltChar;

    std::default_random_engine randomEngine;
    std::uniform_int_distribution<int> uniformIntDistribution(0, saltChar.size() - 1);
    for (int i = 0; i < 16; i++)
    {
        char ch = saltChar.at(uniformIntDistribution(randomEngine)).toLatin1();
        rand16SaltChar.append(ch);
    }

    QString salt = QString("$6$%1$").arg(rand16SaltChar);
    QByteArray saltByteArray = salt.toLatin1();

    char *cryptedResult = nullptr;
    QByteArray cryptedResultBuffer(100, 0);

    //NOTE:兼容低版本libcrypt（不带有crypt_rn接口的版本）
#if 0
    forever
    {
        cryptedResult = crypt_rn(byteArray.data(),
                                 saltByteArray.data(),
                                 cryptedResultBuffer.data(),
                                 cryptedResultBuffer.size());
        if (cryptedResult == nullptr)
        {
            if (errno == ERANGE)
            {
                cryptedResultBuffer.resize(cryptedResultBuffer.size() * 2);
                continue;
            }
            else
            {
                KLOG_WARNING_S() << "encrypt passwd failed," << strerror(errno);
            }
        }
        break;
    }
#else
    crypt_data cryptData{};
    cryptedResult = crypt_r(byteArray.data(),
                            saltByteArray.data(),
                            &cryptData);
#endif

    if (cryptedResult)
        encrypted = cryptedResult;

    return cryptedResult != nullptr;
}

int conv_func(int num_msg, const struct pam_message **msg,
              struct pam_response **resp, void *appdata_ptr)
{
    struct pam_response *reply = NULL;
    int ret;
    int replies;
    char *passwd = (char *)appdata_ptr;

    ///分配回复包
    reply = (struct pam_response *)calloc(num_msg, sizeof(*reply));
    if (reply == nullptr)
    {
        return PAM_CONV_ERR;
    }

    ret = PAM_SUCCESS;
    //给每个ECHO_OFF消息填充密码,若出现ECHO_ON消息认证失败，释放
    for (replies = 0; replies < num_msg && ret == PAM_SUCCESS; replies++)
    {
        if (msg[replies]->msg_style == PAM_PROMPT_ECHO_ON)
        {
            goto failed;
        }
        reply[replies].resp = new char[strlen(passwd) + 1]();
        strcpy(reply[replies].resp, passwd);
        reply[replies].resp_retcode = PAM_SUCCESS;
    }
    *resp = reply;
    return PAM_SUCCESS;

failed:
    ///释放之前分配的内存
    for (int i = 0; i < replies; i++)
    {
        if (reply[i].resp != nullptr)
        {
            delete reply[i].resp;
        }
    }
    free(reply);
    return PAM_CONV_ERR;
}

void no_fail_delay(int status, unsigned int delay, void *appdata_ptr)
{
}

bool PasswdHelper::checkUserPassword(const QString &user, const QString &pwd)
{
    std::string sPwd = pwd.toStdString();
    struct pam_conv conv = {
        &conv_func,
        (void *)sPwd.c_str()};

    pam_handle *handler;
    int res;

    res = pam_start("password-auth", user.toStdString().c_str(),
                    &conv,
                    &handler);

    pam_set_item(handler, PAM_FAIL_DELAY, (void *)no_fail_delay);

    res = pam_authenticate(handler, 0);
    if (res != PAM_SUCCESS)
    {
        KLOG_INFO_S() << pam_strerror(handler, res) << res;
        return false;
    }

    pam_end(handler, res);
    return res == PAM_SUCCESS;
}
