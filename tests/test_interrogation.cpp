#include <gtest/gtest.h>

#include <config_category.h>
#include <plugin_api.h>

#include <utility>
#include <vector>
#include <string>
#include <chrono>

#include "cs104_slave.h"
#include <lib60870/hal_time.h>
#include <lib60870/hal_thread.h>

#include "iec104.h"
#include "iec104_client_config.h"

using namespace std;

#define TEST_PORT 2404

static string protocol_config = QUOTE({
        "protocol_stack" : {
            "name" : "iec104client",
            "version" : "1.0",
            "transport_layer" : {
                "redundancy_groups" : [
                    {
                        "connections" : [
                            {
                                "srv_ip" : "127.0.0.1",
                                "port" : 2404
                            },
                            {
                                "srv_ip" : "127.0.0.1",
                                "port" : 2404
                            }
                        ],
                        "rg_name" : "red-group1",
                        "tls" : false,
                        "k_value" : 12,
                        "w_value" : 8,
                        "t0_timeout" : 10,
                        "t1_timeout" : 15,
                        "t2_timeout" : 10,
                        "t3_timeout" : 20
                    }
                ]
            },
            "application_layer" : {
                "orig_addr" : 10,
                "ca_asdu_size" : 2,
                "ioaddr_size" : 3,
                "asdu_size" : 0,
                "gi_time" : 60,
                "gi_cycle" : 0,
                "gi_all_ca" : true,
                "utc_time" : false,
                "cmd_with_timetag" : false,
                "cmd_parallel" : 0,
                "time_sync" : 100
            },
            "south_monitoring" : {
                "asset": "CONSTAT-1"
            }
        }
    });

static string protocol_config2 = QUOTE({
        "protocol_stack" : {
            "name" : "iec104client",
            "version" : "1.0",
            "transport_layer" : {
                "redundancy_groups" : [
                    {
                        "connections" : [
                            {
                                "srv_ip" : "127.0.0.1",
                                "port" : 2404
                            },
                            {
                                "srv_ip" : "127.0.0.1",
                                "port" : 2404
                            }
                        ],
                        "rg_name" : "red-group1",
                        "tls" : false,
                        "k_value" : 12,
                        "w_value" : 8,
                        "t0_timeout" : 10,
                        "t1_timeout" : 15,
                        "t2_timeout" : 10,
                        "t3_timeout" : 20
                    }
                ]
            },
            "application_layer" : {
                "orig_addr" : 10,
                "ca_asdu_size" : 2,
                "ioaddr_size" : 3,
                "asdu_size" : 0,
                "gi_time" : 60,
                "gi_cycle" : 0,
                "gi_all_ca" : false,
                "utc_time" : false,
                "cmd_with_timetag" : false,
                "cmd_parallel" : 0,
                "time_sync" : 100
            },
            "south_monitoring" : {
                "asset": "CONSTAT-1"
            }
        }
    });

static string protocol_config3 = QUOTE({
        "protocol_stack" : {
            "name" : "iec104client",
            "version" : "1.0",
            "transport_layer" : {
                "redundancy_groups" : [
                    {
                        "connections" : [
                            {
                                "srv_ip" : "127.0.0.1",
                                "port" : 2404
                            }
                        ],
                        "rg_name" : "red-group1",
                        "tls" : false,
                        "k_value" : 12,
                        "w_value" : 8,
                        "t0_timeout" : 10,
                        "t1_timeout" : 15,
                        "t2_timeout" : 10,
                        "t3_timeout" : 20
                    }
                ]
            },
            "application_layer" : {
                "orig_addr" : 10,
                "ca_asdu_size" : 2,
                "ioaddr_size" : 3,
                "asdu_size" : 0,
                "gi_time" : 1,
                "gi_cycle" : 1,
                "gi_all_ca" : false,
                "utc_time" : false,
                "cmd_with_timetag" : false,
                "cmd_parallel" : 0,
                "time_sync" : 100
            },
            "south_monitoring" : {
                "asset": "CONSTAT-1"
            }
        }
    });


static string exchanged_data = QUOTE({
        "exchanged_data": {
            "name" : "iec104client",
            "version" : "1.0",
            "datapoints" : [
                {
                    "label":"TM-1",
                    "protocols":[
                       {
                          "name":"iec104",
                          "address":"41025-4202832",
                          "typeid":"M_ME_NA_1",
                          "gi_groups":"station"
                       }
                    ]
                },
                {
                    "label":"TM-2",
                    "protocols":[
                       {
                          "name":"iec104",
                          "address":"41025-4202852",
                          "typeid":"M_ME_NA_1",
                          "gi_groups":"station"
                       }
                    ]
                },
                {
                    "label":"TS-1",
                    "protocols":[
                       {
                          "name":"iec104",
                          "address":"41025-4206948",
                          "typeid":"M_SP_TB_1",
                          "gi_groups":"station"
                       }
                    ]
                },
                {
                    "label":"TS-2",
                    "pivot_subtypes": [
                        "trigger_south_gi"
                    ],
                    "protocols":[
                       {
                          "name":"iec104",
                          "address":"41025-4206949",
                          "typeid":"M_SP_TB_1",
                          "gi_groups":"station"
                       }
                    ]
                },
                {
                    "label":"TS-3",
                    "pivot_subtypes": [
                        "trigger_south_gi"
                    ],
                    "protocols":[
                       {
                          "name":"iec104",
                          "address":"41025-4206947",
                          "typeid":"M_DP_TB_1",
                          "gi_groups":"station"
                       }
                    ]
                },
                {
                    "label":"C-1",
                    "protocols":[
                       {
                          "name":"iec104",
                          "address":"41025-2000",
                          "typeid":"C_SC_NA_1"
                       }
                    ]
                },
                {
                    "label":"C-2",
                    "protocols":[
                       {
                          "name":"iec104",
                          "address":"41025-2001",
                          "typeid":"C_SC_TA_1"
                       }
                    ]
                },
                {
                    "label":"C-3",
                    "protocols":[
                       {
                          "name":"iec104",
                          "address":"41025-2002",
                          "typeid":"C_DC_NA_1"
                       }
                    ]
                },
                {
                    "label":"TM-B-1",
                    "protocols":[
                       {
                          "name":"iec104",
                          "address":"41026-2001",
                          "typeid":"M_ME_NA_1"
                       }
                    ]
                }
            ]
        }
    });


// PLUGIN DEFAULT TLS CONF
static string tls_config =  QUOTE({
        "tls_conf" : {
            "private_key" : "iec104_client.key",
            "own_cert" : "iec104_client.cer",
            "ca_certs" : [
                {
                    "cert_file": "iec104_ca.cer"
                }
            ],
            "remote_certs" : [
                {
                    "cert_file": "iec104_server.cer"
                }
            ]
        }
    });

class IEC104TestComp : public IEC104
{
public:
    IEC104TestComp() : IEC104()
    {
    }
};

class InterrogationTest : public testing::Test
{
protected:

    IEC104TestComp* iec104 = nullptr;
    int ingestCallbackCalled = 0;
    int southEventIngestCallbackCalled = 0;
    Reading* storedReading;
    Reading* storedSouthEventReading;
    std::vector<Reading*> storedReadings;
    std::vector<Reading*> storedSouthEventReadings;
    int clockSyncHandlerCalled = 0;
    int asduHandlerCalled = 0;
    IMasterConnection lastConnection = nullptr;
    int lastOA = 0;
    int interrogationRequestsReceived = 0;

    void SetUp()
    {
        iec104 = new IEC104TestComp();

        iec104->registerIngest(this, ingestCallback);
    }

    void TearDown()
    {
        iec104->stop();

        delete iec104;

        for (auto reading : storedReadings) {
            delete reading;
        }
    }

    void startIEC104() { iec104->start(); }

    static bool hasChild(Datapoint& dp, std::string childLabel)
    {
        DatapointValue& dpv = dp.getData();

        auto dps = dpv.getDpVec();

        for (auto sdp : *dps) {
            if (sdp->getName() == childLabel) {
                return true;
            }
        }

        return false;
    }

    static Datapoint* getChild(Datapoint& dp, std::string childLabel)
    {
        DatapointValue& dpv = dp.getData();

        auto dps = dpv.getDpVec();

        for (Datapoint* childDp : *dps) {
            if (childDp->getName() == childLabel) {
                return childDp;
            }
        }

        return nullptr;
    }

    static int64_t getIntValue(Datapoint* dp)
    {
        DatapointValue dpValue = dp->getData();
        return dpValue.toInt();
    }

    static std::string getStrValue(Datapoint* dp)
    {
        return dp->getData().toStringValue();
    }

    static bool hasObject(Reading& reading, std::string label)
    {
        std::vector<Datapoint*> dataPoints = reading.getReadingData();

        for (Datapoint* dp : dataPoints)
        {
            if (dp->getName() == label) {
                return true;
            }
        }

        return false;
    }

    static Datapoint* getObject(Reading& reading, std::string label)
    {
        std::vector<Datapoint*> dataPoints = reading.getReadingData();

        for (Datapoint* dp : dataPoints)
        {
            if (dp->getName() == label) {
                return dp;
            }
        }

        return nullptr;
    }

    static bool IsReadingWithQualityInvalid(Reading* reading)
    {
        Datapoint* dataObject = getObject(*reading, "data_object");

        if (dataObject) {
            Datapoint* do_quality_iv = getChild(*dataObject, "do_quality_iv");

            if (do_quality_iv) {
                if (getIntValue(do_quality_iv) == 1) {
                    return true;
                }
            }
        }

        return false;
    }

    static bool IsReadingWithQualityGood(Reading* reading)
    {
        Datapoint* dataObject = getObject(*reading, "data_object");

        if (dataObject) {
            Datapoint* do_quality_iv = getChild(*dataObject, "do_quality_iv");

            if (do_quality_iv) {
                if (getIntValue(do_quality_iv) == 1) {
                    return false;
                }
            }

            Datapoint* do_quality_nt = getChild(*dataObject, "do_quality_nt");

            if (do_quality_nt) {
                if (getIntValue(do_quality_nt) == 1) {
                    return false;
                }
            }

            return true;
        }
        else {
            return false;
        }
    }

    static bool IsReadingWithQualityNonTopcial(Reading* reading)
    {
        Datapoint* dataObject = getObject(*reading, "data_object");

        if (dataObject) {
            Datapoint* do_quality_nt = getChild(*dataObject, "do_quality_nt");

            if (do_quality_nt) {
                if (getIntValue(do_quality_nt) == 1) {
                    return true;
                }
            }
        }

        return false;
    }

    static bool IsConnxStatusStarted(Reading* reading)
    {
        Datapoint* southEvent = getObject(*reading, "south_event");

        if (southEvent) {
            Datapoint* connxStatus = getChild(*southEvent, "connx_status");

            if (connxStatus) {
                if (getStrValue(connxStatus) == "started") {
                    return true;
                }
            }
        }

        return false;
    }

    static bool IsConnxStatusNotConnected(Reading* reading)
    {
        Datapoint* southEvent = getObject(*reading, "south_event");

        if (southEvent) {
            Datapoint* connxStatus = getChild(*southEvent, "connx_status");

            if (connxStatus) {
                if (getStrValue(connxStatus) == "not connected") {
                    return true;
                }
            }
        }

        return false;
    }

    static bool IsGiStatusStarted(Reading* reading)
    {
        Datapoint* southEvent = getObject(*reading, "south_event");

        if (southEvent) {
            Datapoint* connxStatus = getChild(*southEvent, "gi_status");

            if (connxStatus) {
                if (getStrValue(connxStatus) == "started") {
                    return true;
                }
            }
        }

        return false;
    }

    static bool IsGiStatusInProgress(Reading* reading)
    {
        Datapoint* southEvent = getObject(*reading, "south_event");

        if (southEvent) {
            Datapoint* connxStatus = getChild(*southEvent, "gi_status");

            if (connxStatus) {
                if (getStrValue(connxStatus) == "in progress") {
                    return true;
                }
            }
        }

        return false;
    }

    static bool IsGiStatusFailed(Reading* reading)
    {
        Datapoint* southEvent = getObject(*reading, "south_event");

        if (southEvent) {
            Datapoint* connxStatus = getChild(*southEvent, "gi_status");

            if (connxStatus) {
                if (getStrValue(connxStatus) == "failed") {
                    return true;
                }
            }
        }

        return false;
    }

    static bool IsGiStatusFinished(Reading* reading)
    {
        Datapoint* southEvent = getObject(*reading, "south_event");

        if (southEvent) {
            Datapoint* connxStatus = getChild(*southEvent, "gi_status");

            if (connxStatus) {
                if (getStrValue(connxStatus) == "finished") {
                    return true;
                }
            }
        }

        return false;
    }


    static void ingestCallback(void* parameter, Reading reading)
    {
        if(reading.getAssetName() != "CONSTAT-1"){
            InterrogationTest* self = (InterrogationTest*)parameter;
            printf("ingestCallback called -> asset: (%s)\n", reading.getAssetName().c_str());
            std::vector<Datapoint*> dataPoints = reading.getReadingData();
            for (Datapoint* sdp : dataPoints) {
                printf("name: %s value: %s\n", sdp->getName().c_str(), sdp->getData().toString().c_str());
            }
            self->storedReading = new Reading(reading);
            self->storedReadings.push_back(self->storedReading);
            self->ingestCallbackCalled++;
        }
        else{
            InterrogationTest* self = (InterrogationTest*)parameter;
            printf("legacyIngestCallback called -> asset: (%s)\n", reading.getAssetName().c_str());
            std::vector<Datapoint*> dataPoints = reading.getReadingData();
            for (Datapoint* sdp : dataPoints) {
                printf("name: %s value: %s\n", sdp->getName().c_str(), sdp->getData().toString().c_str());
            }
            self->storedSouthEventReading = new Reading(reading);
            self->storedSouthEventReadings.push_back(self->storedSouthEventReading);
            self->southEventIngestCallbackCalled++;
        }
    }


    static bool clockSynchronizationHandler(void* parameter, IMasterConnection connection, CS101_ASDU asdu, CP56Time2a newTime)
    {
        InterrogationTest* self = (InterrogationTest*)parameter;

        char addrBuf[100];

        IMasterConnection_getPeerAddress(connection, addrBuf, 100);

        printf("Clock sync called from %s\n", addrBuf);

        self->clockSyncHandlerCalled++;

        return true;
    }

    static bool asduHandler (void* parameter, IMasterConnection connection, CS101_ASDU asdu)
    {
        InterrogationTest* self = (InterrogationTest*)parameter;

        auto typeId = CS101_ASDU_getTypeID(asdu);
        auto cot = CS101_ASDU_getCOT(asdu);
        printf("asduHandler: type: %s (%d) COT: %s (%d)\n", IEC104ClientConfig::getStringFromTypeID(typeId).c_str(), typeId,
                CS101_CauseOfTransmission_toString(cot), cot);

        self->lastConnection = NULL;
        self->lastOA = CS101_ASDU_getOA(asdu);

        int ca = CS101_ASDU_getCA(asdu);

        InformationObject io = CS101_ASDU_getElement(asdu, 0);

        int ioa = InformationObject_getObjectAddress(io);

        if (typeId == C_SC_NA_1) {
            printf("  C_SC_NA_1 (single-command)\n");

            if (ca == 41025 && ioa == 2000) {
                IMasterConnection_sendACT_CON(connection, asdu, false);
                self->lastConnection = connection;
            }


        }
        else if (typeId == C_SC_TA_1) {
            printf("  C_SC_TA_1 (single-command w/timetag)\n");

            if (ca == 41025 && ioa == 2001) {
                IMasterConnection_sendACT_CON(connection, asdu, false);
                self->lastConnection = connection;
            }
        }
        else if (typeId == C_DC_NA_1) {
            printf("  C_DC_NA_1 (double-command)\n");

            if (ca == 41025 && ioa == 2002) {
                IMasterConnection_sendACT_CON(connection, asdu, false);
                self->lastConnection = connection;
            }
        }

        self->asduHandlerCalled++;

        return true;
    }

    static bool interrogationHandler(void* parameter, IMasterConnection connection, CS101_ASDU asdu, uint8_t qoi)
    {
        InterrogationTest* self = (InterrogationTest*)parameter;

        self->interrogationRequestsReceived++;

        printf("CA=%i Received interrogation for group %i\n", CS101_ASDU_getCA(asdu), qoi);

        if (qoi == 20) { /* only handle station interrogation */

            CS101_AppLayerParameters alParams = IMasterConnection_getApplicationLayerParameters(connection);

            IMasterConnection_sendACT_CON(connection, asdu, false);

            /* The CS101 specification only allows information objects without timestamp in GI responses */

            CS101_ASDU newAsdu = CS101_ASDU_create(alParams, false, CS101_COT_INTERROGATED_BY_STATION,
                    0, 1, false, false);

            InformationObject io = (InformationObject) MeasuredValueScaled_create(NULL, 100, -1, IEC60870_QUALITY_GOOD);

            CS101_ASDU_addInformationObject(newAsdu, io);

            CS101_ASDU_addInformationObject(newAsdu, (InformationObject)
                MeasuredValueScaled_create((MeasuredValueScaled) io, 101, 23, IEC60870_QUALITY_GOOD));

            CS101_ASDU_addInformationObject(newAsdu, (InformationObject)
                MeasuredValueScaled_create((MeasuredValueScaled) io, 102, 2300, IEC60870_QUALITY_GOOD));

            InformationObject_destroy(io);

            IMasterConnection_sendASDU(connection, newAsdu);

            CS101_ASDU_destroy(newAsdu);

            newAsdu = CS101_ASDU_create(alParams, false, CS101_COT_INTERROGATED_BY_STATION,
                        0, 1, false, false);

            io = (InformationObject) SinglePointInformation_create(NULL, 104, true, IEC60870_QUALITY_GOOD);

            CS101_ASDU_addInformationObject(newAsdu, io);

            CS101_ASDU_addInformationObject(newAsdu, (InformationObject)
                SinglePointInformation_create((SinglePointInformation) io, 105, false, IEC60870_QUALITY_GOOD));

            InformationObject_destroy(io);

            IMasterConnection_sendASDU(connection, newAsdu);

            CS101_ASDU_destroy(newAsdu);

            newAsdu = CS101_ASDU_create(alParams, true, CS101_COT_INTERROGATED_BY_STATION,
                    0, 1, false, false);

            CS101_ASDU_addInformationObject(newAsdu, io = (InformationObject) SinglePointInformation_create(NULL, 300, true, IEC60870_QUALITY_GOOD));
            CS101_ASDU_addInformationObject(newAsdu, (InformationObject) SinglePointInformation_create((SinglePointInformation) io, 301, false, IEC60870_QUALITY_GOOD));
            CS101_ASDU_addInformationObject(newAsdu, (InformationObject) SinglePointInformation_create((SinglePointInformation) io, 302, true, IEC60870_QUALITY_GOOD));
            CS101_ASDU_addInformationObject(newAsdu, (InformationObject) SinglePointInformation_create((SinglePointInformation) io, 303, false, IEC60870_QUALITY_GOOD));
            CS101_ASDU_addInformationObject(newAsdu, (InformationObject) SinglePointInformation_create((SinglePointInformation) io, 304, true, IEC60870_QUALITY_GOOD));
            CS101_ASDU_addInformationObject(newAsdu, (InformationObject) SinglePointInformation_create((SinglePointInformation) io, 305, false, IEC60870_QUALITY_GOOD));
            CS101_ASDU_addInformationObject(newAsdu, (InformationObject) SinglePointInformation_create((SinglePointInformation) io, 306, true, IEC60870_QUALITY_GOOD));
            CS101_ASDU_addInformationObject(newAsdu, (InformationObject) SinglePointInformation_create((SinglePointInformation) io, 307, false, IEC60870_QUALITY_GOOD));

            InformationObject_destroy(io);

            IMasterConnection_sendASDU(connection, newAsdu);

            CS101_ASDU_destroy(newAsdu);

            newAsdu = CS101_ASDU_create(alParams, false, CS101_COT_INTERROGATED_BY_STATION,
                            0, 1, false, false);

            io = (InformationObject) BitString32_create(NULL, 500, 0xaaaa);

            CS101_ASDU_addInformationObject(newAsdu, io);

            InformationObject_destroy(io);

            IMasterConnection_sendASDU(connection, newAsdu);

            CS101_ASDU_destroy(newAsdu);

            IMasterConnection_sendACT_TERM(connection, asdu);
        }
        else {
            IMasterConnection_sendACT_CON(connection, asdu, true);
        }

        return true;
    }

    static bool interrogationHandler_configuredDatapoints(void* parameter, IMasterConnection connection, CS101_ASDU asdu, uint8_t qoi)
    {
        InterrogationTest* self = (InterrogationTest*)parameter;

        self->interrogationRequestsReceived++;

        printf("CA=%i Received interrogation for group %i\n", CS101_ASDU_getCA(asdu), qoi);

        if (qoi == 20) { /* only handle station interrogation */

            CS101_AppLayerParameters alParams = IMasterConnection_getApplicationLayerParameters(connection);

            IMasterConnection_sendACT_CON(connection, asdu, false);

            /* The CS101 specification only allows information objects without timestamp in GI responses */

            CS101_ASDU newAsdu = CS101_ASDU_create(alParams, false, CS101_COT_INTERROGATED_BY_STATION,
                    0, 41025, false, false);

            InformationObject io = (InformationObject) MeasuredValueNormalized_create(NULL, 4202832, 0.5, IEC60870_QUALITY_GOOD);

            CS101_ASDU_addInformationObject(newAsdu, io);

            CS101_ASDU_addInformationObject(newAsdu, (InformationObject)
                MeasuredValueNormalized_create((MeasuredValueNormalized) io, 4202852, 0.6, IEC60870_QUALITY_GOOD));


            InformationObject_destroy(io);

            IMasterConnection_sendASDU(connection, newAsdu);

            CS101_ASDU_destroy(newAsdu);

            newAsdu = CS101_ASDU_create(alParams, false, CS101_COT_INTERROGATED_BY_STATION, 0, 41025, false, false);
            CS101_ASDU_addInformationObject(newAsdu, (InformationObject) DoublePointInformation_create(NULL, 4206947, DoublePointValue::IEC60870_DOUBLE_POINT_ON, IEC60870_QUALITY_GOOD));
            IMasterConnection_sendASDU(connection, newAsdu);
            CS101_ASDU_destroy(newAsdu);
            newAsdu = CS101_ASDU_create(alParams, false, CS101_COT_INTERROGATED_BY_STATION, 0, 41025, false, false);
            CS101_ASDU_addInformationObject(newAsdu, (InformationObject) SinglePointInformation_create(NULL, 4206948, true, IEC60870_QUALITY_GOOD));
            IMasterConnection_sendASDU(connection, newAsdu);
            CS101_ASDU_destroy(newAsdu);
            newAsdu = CS101_ASDU_create(alParams, false, CS101_COT_INTERROGATED_BY_STATION, 0, 41025, false, false);
            CS101_ASDU_addInformationObject(newAsdu, (InformationObject) SinglePointInformation_create(NULL, 4206949, true, IEC60870_QUALITY_GOOD));
            IMasterConnection_sendASDU(connection, newAsdu);
            CS101_ASDU_destroy(newAsdu);

            IMasterConnection_sendACT_TERM(connection, asdu);
        }
        else {
            IMasterConnection_sendACT_CON(connection, asdu, true);
        }

        return true;
    }

    static bool interrogationHandler_No_ACT_CON(void* parameter, IMasterConnection connection, CS101_ASDU asdu, uint8_t qoi)
    {
        InterrogationTest* self = (InterrogationTest*)parameter;

        self->interrogationRequestsReceived++;

        printf("Received interrogation for group %i\n", qoi);

        return true;
    }

    bool
    containsString(vector<string> array, string str){
        return std::find(array.begin(),array.end(),str) != array.end();
    }


    bool 
    containSouthEventsInRightOrder(vector<Reading*> readings, vector<string> expected_unique_events){
        vector<string> unique_events;

        for(Reading* reading : readings){
            Datapoint* south_event = reading->getReadingData().at(0);
            string south_event_value = south_event->getData().toString();

            if(!containsString(unique_events,south_event_value)){
                unique_events.push_back(south_event_value);
            }    
        }

        return unique_events == expected_unique_events;
    }
};

TEST_F(InterrogationTest, IEC104Client_startupProcedureSeparateRequestForEachCA)
{
    iec104->setJsonConfig(protocol_config, exchanged_data, tls_config);

    asduHandlerCalled = 0;
    interrogationRequestsReceived = 0;
    clockSyncHandlerCalled = 0;
    lastConnection = NULL;
    ingestCallbackCalled = 0;

    CS104_Slave slave = CS104_Slave_create(10, 10);
    ASSERT_NE(slave, nullptr);

    CS104_Slave_setLocalPort(slave, TEST_PORT);

    CS104_Slave_setClockSyncHandler(slave, clockSynchronizationHandler, this);
    CS104_Slave_setASDUHandler(slave, asduHandler, this);
    CS104_Slave_setInterrogationHandler(slave, interrogationHandler, this);

    CS104_Slave_start(slave);

    CS101_AppLayerParameters alParams = CS104_Slave_getAppLayerParameters(slave);

    startIEC104();

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    int duration = 0;
    while (interrogationRequestsReceived < 2 && duration < 1500)
    {
        Thread_sleep(10);
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    ASSERT_NE(true , duration >= 1500);

    CS104_Slave_stop(slave);

    CS104_Slave_destroy(slave);

    Thread_sleep(500);
}

TEST_F(InterrogationTest, IEC104Client_startupProcedureBroadcastCA)
{
    iec104->setJsonConfig(protocol_config2, exchanged_data, tls_config);

    asduHandlerCalled = 0;
    interrogationRequestsReceived = 0;
    clockSyncHandlerCalled = 0;
    lastConnection = NULL;
    ingestCallbackCalled = 0;

    CS104_Slave slave = CS104_Slave_create(10, 10);
    ASSERT_NE(slave, nullptr);

    CS104_Slave_setLocalPort(slave, TEST_PORT);

    CS104_Slave_setClockSyncHandler(slave, clockSynchronizationHandler, this);
    CS104_Slave_setASDUHandler(slave, asduHandler, this);
    CS104_Slave_setInterrogationHandler(slave, interrogationHandler, this);

    CS104_Slave_start(slave);

    CS101_AppLayerParameters alParams = CS104_Slave_getAppLayerParameters(slave);

    startIEC104();

    ASSERT_EQ(0, asduHandlerCalled);

    Thread_sleep(2500);

    ASSERT_EQ(1, interrogationRequestsReceived);

    CS104_Slave_stop(slave);

    CS104_Slave_destroy(slave);

    Thread_sleep(500);
}

TEST_F(InterrogationTest, IEC104Client_GIcycleOneSecond)
{
    iec104->setJsonConfig(protocol_config3, exchanged_data, tls_config);

    asduHandlerCalled = 0;
    interrogationRequestsReceived = 0;
    clockSyncHandlerCalled = 0;
    lastConnection = NULL;
    ingestCallbackCalled = 0;

    CS104_Slave slave = CS104_Slave_create(10, 10);
    ASSERT_NE(slave, nullptr);

    CS104_Slave_setLocalPort(slave, TEST_PORT);

    CS104_Slave_setClockSyncHandler(slave, clockSynchronizationHandler, this);
    CS104_Slave_setASDUHandler(slave, asduHandler, this);
    CS104_Slave_setInterrogationHandler(slave, interrogationHandler, this);

    CS104_Slave_start(slave);

    CS101_AppLayerParameters alParams = CS104_Slave_getAppLayerParameters(slave);

    startIEC104();

    Thread_sleep(800);

    ASSERT_EQ(1, clockSyncHandlerCalled);
    ASSERT_EQ(1, interrogationRequestsReceived);

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();

    int duration = 0;
    while (interrogationRequestsReceived < 3 && duration < 4000)
    {
        Thread_sleep(10);
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    ASSERT_NE(true , duration >= 4000);

    CS104_Slave_stop(slave);

    CS104_Slave_destroy(slave);

    Thread_sleep(500);
}


TEST_F(InterrogationTest, IEC104Client_GIcycleOneSecondNoACT_CON)
{
    iec104->setJsonConfig(protocol_config3, exchanged_data, tls_config);

    asduHandlerCalled = 0;
    interrogationRequestsReceived = 0;
    clockSyncHandlerCalled = 0;
    lastConnection = NULL;
    ingestCallbackCalled = 0;

    CS104_Slave slave = CS104_Slave_create(10, 10);
    ASSERT_NE(slave, nullptr);

    CS104_Slave_setLocalPort(slave, TEST_PORT);

    CS104_Slave_setClockSyncHandler(slave, clockSynchronizationHandler, this);
    CS104_Slave_setASDUHandler(slave, asduHandler, this);
    CS104_Slave_setInterrogationHandler(slave, interrogationHandler_No_ACT_CON, this);

    CS104_Slave_start(slave);

    CS101_AppLayerParameters alParams = CS104_Slave_getAppLayerParameters(slave);

    startIEC104();

    Thread_sleep(500);

    ASSERT_EQ(1, clockSyncHandlerCalled);
    ASSERT_EQ(0, asduHandlerCalled);
    ASSERT_EQ(1, interrogationRequestsReceived);

    Thread_sleep(2000);

    ASSERT_EQ(1, interrogationRequestsReceived);

    CS104_Slave_stop(slave);

    CS104_Slave_destroy(slave);

    Thread_sleep(500);
}

TEST_F(InterrogationTest, InterrogationRequestAfter_M_EI_NA_1)
{
    iec104->setJsonConfig(protocol_config2, exchanged_data, tls_config);

    asduHandlerCalled = 0;
    interrogationRequestsReceived = 0;
    clockSyncHandlerCalled = 0;
    lastConnection = NULL;
    ingestCallbackCalled = 0;

    CS104_Slave slave = CS104_Slave_create(10, 10);
    ASSERT_NE(slave, nullptr);

    CS104_Slave_setLocalPort(slave, TEST_PORT);

    CS104_Slave_setClockSyncHandler(slave, clockSynchronizationHandler, this);
    CS104_Slave_setASDUHandler(slave, asduHandler, this);
    CS104_Slave_setInterrogationHandler(slave, interrogationHandler, this);

    CS104_Slave_start(slave);

    CS101_AppLayerParameters alParams = CS104_Slave_getAppLayerParameters(slave);

    startIEC104();

    Thread_sleep(500);

    ASSERT_EQ(0, asduHandlerCalled);

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    int duration = 0;

    while (interrogationRequestsReceived < 1 && duration < 1500)
    {
        Thread_sleep(10);
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    ASSERT_NE(true , duration >= 1500);

    CS101_ASDU asdu = CS101_ASDU_create(alParams, false, CS101_COT_INITIALIZED,
                    0, 1, false, false);

    InformationObject io = (InformationObject)EndOfInitialization_create(NULL, 0);

    CS101_ASDU_addInformationObject(asdu, io);

    InformationObject_destroy(io);

    CS104_Slave_enqueueASDU(slave, asdu);

    printf("[%lu]Send M_EI_NA_1\n", Hal_getTimeInMs());

    CS101_ASDU_destroy(asdu);

    Thread_sleep(1500);

    CS104_Slave_stop(slave);

    CS104_Slave_destroy(slave);

    Thread_sleep(500);

    vector<string> expected_unique_events;

    expected_unique_events.push_back("{\"connx_status\":\"started\"}");

    expected_unique_events.push_back("{\"gi_status\":\"started\"}");

    expected_unique_events.push_back("{\"gi_status\":\"in progress\"}");

    expected_unique_events.push_back("{\"gi_status\":\"finished\"}");

    expected_unique_events.push_back("{\"connx_status\":\"not connected\"}");

    ASSERT_TRUE(containSouthEventsInRightOrder(storedSouthEventReadings,expected_unique_events));

    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[0]));
    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[1]));
    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[2]));
    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[3]));

    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[4]));
    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[5]));
    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[6]));

    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[7]));
    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[8]));
    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[9]));

}

TEST_F(InterrogationTest, GICycleReceiveConfiguredDatapoints)
{
    iec104->setJsonConfig(protocol_config3, exchanged_data, tls_config);

    asduHandlerCalled = 0;
    interrogationRequestsReceived = 0;
    clockSyncHandlerCalled = 0;
    lastConnection = NULL;
    ingestCallbackCalled = 0;

    CS104_Slave slave = CS104_Slave_create(10, 10);
    ASSERT_NE(slave, nullptr);

    CS104_Slave_setLocalPort(slave, TEST_PORT);

    CS104_Slave_setClockSyncHandler(slave, clockSynchronizationHandler, this);
    CS104_Slave_setASDUHandler(slave, asduHandler, this);
    CS104_Slave_setInterrogationHandler(slave, interrogationHandler_configuredDatapoints, this);

    CS104_Slave_start(slave);

    CS101_AppLayerParameters alParams = CS104_Slave_getAppLayerParameters(slave);


    startIEC104();

    Thread_sleep(500);

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    int duration = 0;

    while (interrogationRequestsReceived < 3 && duration < 3000)
    {
        Thread_sleep(10);
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    ASSERT_NEAR(2000 , duration, 1500);

    CS104_Slave_stop(slave);

    CS104_Slave_destroy(slave);

    Thread_sleep(500);

    vector<string> expected_unique_events;

    expected_unique_events.push_back("{\"connx_status\":\"started\"}");

    expected_unique_events.push_back("{\"gi_status\":\"started\"}");

    expected_unique_events.push_back("{\"gi_status\":\"in progress\"}");

    expected_unique_events.push_back("{\"gi_status\":\"finished\"}");

    expected_unique_events.push_back("{\"connx_status\":\"not connected\"}");

    ASSERT_TRUE(containSouthEventsInRightOrder(storedSouthEventReadings,expected_unique_events));

    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[0])); // TM-1
    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[1])); // TM-2
    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[2])); // TS-1
    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[3])); // TS-2
    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[4])); // TS-3
    ASSERT_TRUE(IsReadingWithQualityInvalid(storedReadings[5])); // TM-B-1

    ASSERT_TRUE(IsReadingWithQualityGood(storedReadings[6])); // TM-1
    ASSERT_TRUE(IsReadingWithQualityGood(storedReadings[7])); // TM-2
    ASSERT_TRUE(IsReadingWithQualityGood(storedReadings[8])); // TM-1
    ASSERT_TRUE(IsReadingWithQualityGood(storedReadings[9])); // TS-1
    ASSERT_TRUE(IsReadingWithQualityGood(storedReadings[10])); // TS-2
    ASSERT_TRUE(IsReadingWithQualityGood(storedReadings[11])); // TS-3

    ASSERT_TRUE(IsReadingWithQualityGood(storedReadings[12])); // TM-1
    ASSERT_TRUE(IsReadingWithQualityGood(storedReadings[13])); // TM-2
    ASSERT_TRUE(IsReadingWithQualityGood(storedReadings[14])); // TM-1
    ASSERT_TRUE(IsReadingWithQualityGood(storedReadings[15])); // TS-1
    ASSERT_TRUE(IsReadingWithQualityGood(storedReadings[16])); // TS-2
    ASSERT_TRUE(IsReadingWithQualityGood(storedReadings[17])); // TS-3

}

TEST_F(InterrogationTest, InterrogationRequestAfterExpPart)
{
    /* #################################################################
    ** #####                                                       #####
    ** #####    First we connect our devices and perform a GI      #####
    ** #####                                                       #####
    ** #################################################################
    */

    iec104->setJsonConfig(protocol_config2, exchanged_data, tls_config);

    asduHandlerCalled = 0;
    interrogationRequestsReceived = 0;
    clockSyncHandlerCalled = 0;
    lastConnection = NULL;
    ingestCallbackCalled = 0;

    CS104_Slave slave = CS104_Slave_create(10, 10);
    ASSERT_NE(slave, nullptr);
    CS104_Slave_setLocalPort(slave, TEST_PORT);
    CS104_Slave_setClockSyncHandler(slave, clockSynchronizationHandler, this);
    CS104_Slave_setASDUHandler(slave, asduHandler, this);
    CS104_Slave_setInterrogationHandler(slave, interrogationHandler, this);
    CS104_Slave_start(slave);
    startIEC104();

    // End start GI by sending M_EI_NA
    CS101_ASDU asdu = CS101_ASDU_create(CS104_Slave_getAppLayerParameters(slave), false, CS101_COT_INITIALIZED, 0, 1, false, false);
    InformationObject io = (InformationObject)EndOfInitialization_create(NULL, 0);
    CS101_ASDU_addInformationObject(asdu, io);
    InformationObject_destroy(io);
    CS104_Slave_enqueueASDU(slave, asdu);
    CS101_ASDU_destroy(asdu);
    
    printf("[%lu]Send M_EI_NA_1\n", Hal_getTimeInMs());

    Thread_sleep(500);
    ASSERT_EQ(1, interrogationRequestsReceived);
    interrogationRequestsReceived = 0;

    /* ##################################################################
    ** #####                                                        #####
    ** ##### Then we send a normal TSCE that shouldn't trigger a GI #####
    ** #####                                                        #####
    ** ##################################################################
    */

    CS101_ASDU newAsdu = CS101_ASDU_create(CS104_Slave_getAppLayerParameters(slave), false, CS101_COT_SPONTANEOUS, 0, 41025, false, false);
    struct sCP56Time2a ts;
    uint64_t timestamp = Hal_getTimeInMs();
    CP56Time2a_createFromMsTimestamp(&ts, timestamp);
    io = (InformationObject) SinglePointWithCP56Time2a_create(NULL, 4206948, false, IEC60870_QUALITY_GOOD, &ts);
    CS101_ASDU_addInformationObject(newAsdu, io);
    InformationObject_destroy(io);
    CS104_Slave_enqueueASDU(slave, newAsdu);
    CS101_ASDU_destroy(newAsdu);

    Thread_sleep(500);
    ASSERT_EQ(0, interrogationRequestsReceived);

    /* ##################################################################
    ** #####                                                        #####
    ** #####        Then we send a GI triggering STSCE to 1         #####
    ** #####                                                        #####
    ** ##################################################################
    */

    newAsdu = CS101_ASDU_create(CS104_Slave_getAppLayerParameters(slave), false, CS101_COT_SPONTANEOUS, 0, 41025, false, false);
    timestamp = Hal_getTimeInMs();
    CP56Time2a_createFromMsTimestamp(&ts, timestamp);
    io = (InformationObject) SinglePointWithCP56Time2a_create(NULL, 4206949, true, IEC60870_QUALITY_GOOD, &ts);
    CS101_ASDU_addInformationObject(newAsdu, io);
    InformationObject_destroy(io);
    CS104_Slave_enqueueASDU(slave, newAsdu);
    CS101_ASDU_destroy(newAsdu);

    Thread_sleep(500);
    ASSERT_EQ(0, interrogationRequestsReceived);

    /* ##################################################################
    ** #####                                                        #####
    ** #####        Then we send a GI triggering STSCE to 0         #####
    ** #####                                                        #####
    ** ##################################################################
    */

    newAsdu = CS101_ASDU_create(CS104_Slave_getAppLayerParameters(slave), false, CS101_COT_SPONTANEOUS, 0, 41025, false, false);
    timestamp = Hal_getTimeInMs();
    CP56Time2a_createFromMsTimestamp(&ts, timestamp);
    io = (InformationObject) SinglePointWithCP56Time2a_create(NULL, 4206949, false, IEC60870_QUALITY_GOOD, &ts);
    CS101_ASDU_addInformationObject(newAsdu, io);
    InformationObject_destroy(io);
    CS104_Slave_enqueueASDU(slave, newAsdu);
    CS101_ASDU_destroy(newAsdu);

    Thread_sleep(500);
    ASSERT_EQ(1, interrogationRequestsReceived);
    interrogationRequestsReceived = 0;

    /* ##################################################################
    ** #####                                                        #####
    ** #####       Then we send a GI triggering DTSCE to 0.5        #####
    ** #####                                                        #####
    ** ##################################################################
    */

    newAsdu = CS101_ASDU_create(CS104_Slave_getAppLayerParameters(slave), false, CS101_COT_SPONTANEOUS, 0, 41025, false, false);
    timestamp = Hal_getTimeInMs();
    CP56Time2a_createFromMsTimestamp(&ts, timestamp);
    io = (InformationObject) DoublePointWithCP56Time2a_create(NULL, 4206947, DoublePointValue::IEC60870_DOUBLE_POINT_INDETERMINATE, IEC60870_QUALITY_GOOD, &ts);
    CS101_ASDU_addInformationObject(newAsdu, io);
    InformationObject_destroy(io);
    CS104_Slave_enqueueASDU(slave, newAsdu);
    CS101_ASDU_destroy(newAsdu);

    Thread_sleep(500);
    ASSERT_EQ(0, interrogationRequestsReceived);

    /* ##################################################################
    ** #####                                                        #####
    ** #####        Then we send a GI triggering DTSCE to 1         #####
    ** #####                                                        #####
    ** ##################################################################
    */

    newAsdu = CS101_ASDU_create(CS104_Slave_getAppLayerParameters(slave), false, CS101_COT_SPONTANEOUS, 0, 41025, false, false);
    timestamp = Hal_getTimeInMs();
    CP56Time2a_createFromMsTimestamp(&ts, timestamp);
    io = (InformationObject) DoublePointWithCP56Time2a_create(NULL, 4206947, DoublePointValue::IEC60870_DOUBLE_POINT_ON, IEC60870_QUALITY_GOOD, &ts);
    CS101_ASDU_addInformationObject(newAsdu, io);
    InformationObject_destroy(io);
    CS104_Slave_enqueueASDU(slave, newAsdu);
    CS101_ASDU_destroy(newAsdu);

    Thread_sleep(500);
    ASSERT_EQ(0, interrogationRequestsReceived);

    /* ##################################################################
    ** #####                                                        #####
    ** #####        Then we send a GI triggering DTSCE to 0         #####
    ** #####                                                        #####
    ** ##################################################################
    */

    newAsdu = CS101_ASDU_create(CS104_Slave_getAppLayerParameters(slave), false, CS101_COT_SPONTANEOUS, 0, 41025, false, false);
    timestamp = Hal_getTimeInMs();
    CP56Time2a_createFromMsTimestamp(&ts, timestamp);
    io = (InformationObject) DoublePointWithCP56Time2a_create(NULL, 4206947, DoublePointValue::IEC60870_DOUBLE_POINT_OFF, IEC60870_QUALITY_GOOD, &ts);
    CS101_ASDU_addInformationObject(newAsdu, io);
    InformationObject_destroy(io);
    CS104_Slave_enqueueASDU(slave, newAsdu);
    CS101_ASDU_destroy(newAsdu);

    Thread_sleep(500);
    ASSERT_EQ(1, interrogationRequestsReceived);
    interrogationRequestsReceived = 0;

    /* ##################################################################
    ** #####                                                        #####
    ** #####      Then we send three GI triggering STSCE to 0       #####
    ** #####                                                        #####
    ** ##################################################################
    */

    newAsdu = CS101_ASDU_create(CS104_Slave_getAppLayerParameters(slave), false, CS101_COT_SPONTANEOUS, 0, 41025, false, false);
    timestamp = Hal_getTimeInMs();
    CP56Time2a_createFromMsTimestamp(&ts, timestamp);
    io = (InformationObject) SinglePointWithCP56Time2a_create(NULL, 4206949, false, IEC60870_QUALITY_GOOD, &ts);
    CS101_ASDU_addInformationObject(newAsdu, io);
    InformationObject_destroy(io);

    CS104_Slave_enqueueASDU(slave, newAsdu);
    Thread_sleep(100); // EXP.PART received in less than 50ms trigger 1 GI
    CS104_Slave_enqueueASDU(slave, newAsdu);
    Thread_sleep(100); // EXP.PART received in less than 50ms trigger 1 GI
    CS104_Slave_enqueueASDU(slave, newAsdu);
    CS101_ASDU_destroy(newAsdu);

    Thread_sleep(1500);
    ASSERT_EQ(2, interrogationRequestsReceived);
    interrogationRequestsReceived = 0;

    // Clean slave
    CS104_Slave_stop(slave);
    CS104_Slave_destroy(slave);
}
