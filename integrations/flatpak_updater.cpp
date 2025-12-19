#include "core.h"
#include "entities/entities.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>
#include <QVariantList>
#include <QUrl>
#include <QRegularExpression>
#include <QEventLoop>
#include <KSandbox>

#include <KSharedConfig>
#include <KConfigGroup>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(auf)
Q_LOGGING_CATEGORY(auf, "integration.AutoUpdater-Flatpak")

class FlatpakUpdater : public QObject
{
    Q_OBJECT
public:
    explicit FlatpakUpdater(QObject *parent = nullptr) : QObject(parent)
    {
        // TODO add the update entity so its available from HA
        m_updater = new Update(this);
        m_updater->setName("KIOT Flatpak Updater");
        m_updater->setId("flatpak_updates");
        m_updater->setInstalledVersion(QStringLiteral(KIOT_VERSION));

       
        connect(m_updater, &Update::installRequested, this, &FlatpakUpdater::update);

        //Reads the config file to get the timestamp for last time we checked for å update
        config = KSharedConfig::openConfig();
        updaterGroup = config->group("Updater");
        lastCheck = updaterGroup.readEntry("LastCheck", QDateTime());

        // TODO start timer to check for updates and run a manual check on startup
        lastRepoData = fetchLatestRelease(repo_url);
        // TODO get latest version from github release
        m_updater->setLatestVersion(QStringLiteral(KIOT_VERSION));
        m_updater->setReleaseSummary("Flatpak updates for kiot comming soon"); 
        m_updater->setTitle("kiot flatpak");
        m_updater->setReleaseUrl("https://github.com/davidedmundson/kiot/releases");
    }


    ~FlatpakUpdater()
    {

    }

    void update()
    {
        //TODO download latest release and install it before we do a restart 
    }

    void checkForUpdates(){
        lastCheck = updaterGroup.readEntry("LastCheck", QDateTime());

        // Check to be sure we dont spam the github api
        int time = 86400; // 24 hours
        if (lastCheck.isValid() && lastCheck.secsTo(QDateTime::currentDateTimeUtc()) < time)
            return;
        qCDebug(auf) << "Checking for updates";
        // TODO call fetchLatestRelease from repo url and compare with our installed info
        // set update entity with latest release info if its mewer than installed version
        // if update entity is newer than current version, set update available
 
        updaterGroup.writeEntry("LastCheck", QDateTime::currentDateTimeUtc());
        config->sync();
    }

    //Grabs latest release info from github so we can check if there is a new release
    QVariantMap fetchLatestRelease(const QString &repoUrl)
    {
        QNetworkAccessManager manager;

        QRegularExpression re(R"(github\.com/([^/]+)/([^/]+))");
        auto match = re.match(repoUrl);
        if (!match.hasMatch())
            return {};

        const QString owner = match.captured(1);
        const QString repo  = match.captured(2);

        const QUrl apiUrl(QStringLiteral("https://api.github.com/repos/%1/%2/releases/latest").arg(owner, repo));

        QNetworkRequest request(apiUrl);
        request.setHeader(QNetworkRequest::UserAgentHeader, "Kiot-Updater");

        QNetworkReply *reply = manager.get(request);

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            return {};
        }

        const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        reply->deleteLater();

        if (!doc.isObject())
            return {};

        const QJsonObject obj = doc.object();

        QVariantMap result;
        result["tag_name"]     = obj.value("tag_name").toString();
        result["name"]         = obj.value("name").toString();
        result["published_at"] = obj.value("published_at").toString();
        result["html_url"]     = obj.value("html_url").toString();
        result["body"]         = obj.value("body").toString();

        QVariantList assetsList;
        const QJsonArray assets = obj.value("assets").toArray();

        for (const QJsonValue &val : assets) {
            const QJsonObject assetObj = val.toObject();

            QVariantMap asset;
            asset["name"]                 = assetObj.value("name").toString();
            asset["size"]                 = assetObj.value("size").toInt();
            asset["content_type"]         = assetObj.value("content_type").toString();
            asset["browser_download_url"] = assetObj.value("browser_download_url").toString();
            asset["download_count"]       = assetObj.value("download_count").toInt();

            assetsList.append(asset);
        }

        result["assets"] = assetsList;

        return result;
    }

private:
    KSharedConfig::Ptr config;
    KConfigGroup updaterGroup;
    QDateTime lastCheck;
    QString repo_url = "";
    QVariantMap lastRepoData;
    Update *m_updater;
};



void setupFlatpakUpdater()
{
    if (!KSandbox::isFlatpak()) 
    {
        qCWarning(auf) << "FlatpakUpdater is only supported in Flatpak environments,aborting";
        return;
    }
    new FlatpakUpdater(qApp);
}

REGISTER_INTEGRATION("UpdaterFlatpak", setupFlatpakUpdater, false)
#include "flatpak_updater.moc"
