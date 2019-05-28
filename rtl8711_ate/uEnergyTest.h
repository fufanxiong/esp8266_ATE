/*******************************************************************************

    FILE:       uEnergyTest.h
 
                Copyright Cambridge Silicon Radio Limited 2011 - 2015.
                Confidential. http://www.csr.com.
 
    PURPOSE:    Defines the uEnergyTest DLL API, used for production testing
                of CSR uEnergy devices.

*******************************************************************************/

#ifndef UENERGYTEST_H
#define UENERGYTEST_H

#include "types.h"

#ifdef WIN32
// Note: Not using declspec(dllexport/dllimport) here as the def file defines the exports.
#ifdef UENERGYTEST_DLL
# define UENERGYTEST_API(T) T __stdcall
#else
# define UENERGYTEST_API(T) T __stdcall
#endif
#else
# define UENERGYTEST_API(T) T
#endif // WIN32

// uEnergyTest result codes
const int32 UET_WARNING                       =   1; // The call succeeded but there was a warning (use uetGetLastError to get details)
const int32 UET_OK                            =   0; // Success
const int32 UET_ERR_GENERAL                   =  -1; // General error code
const int32 UET_ERR_UNSUPPORTED_FUNCTION      =  -2; // Unsupported function called
const int32 UET_ERR_INVALID_HANDLE            =  -3; // Invalid device handle
const int32 UET_ERR_NO_MEM                    =  -4; // Out of memory
const int32 UET_ERR_TEST_FAILED               =  -5; // Test failed (for functions that actually perform tests)
const int32 UET_ERR_SEQUENCE                  =  -6; // The sequence of API calls is incorrect
const int32 UET_ERR_PARAM_INVALID             = -10; // Invalid parameter(s) passed to the API
const int32 UET_ERR_PARAM_TOO_SMALL           = -11; // A buffer (array) parameter is too small to contain the requested data
const int32 UET_ERR_DUT_CONN                  = -20; // Device connection error
const int32 UET_ERR_DUT_UNSUPPORTED           = -21; // The connected device is not supported
const int32 UET_ERR_DUT_FAILED_TO_STOP        = -22; // Device failed to stop
const int32 UET_ERR_DUT_WRITE                 = -23; // Write to device failed
const int32 UET_ERR_DUT_READ                  = -24; // Read from device failed
const int32 UET_ERR_DUT_FAILED_TO_START       = -25; // Device failed to start
const int32 UET_ERR_FW_CMD_UNSPEC             = -30; // Unspecified error for firmware command
const int32 UET_ERR_FW_CMD                    = -31; // Firmware command failed
const int32 UET_ERR_FW_INIT                   = -32; // Firmware failed to initialise
const int32 UET_ERR_FW_TIMEOUT                = -33; // Timeout waiting for firmware
const int32 UET_ERR_FW_XBV                    = -34; // Firmware XVB file error
const int32 UET_ERR_CS_CACHE_GENERAL          = -40; // CS cache general error
const int32 UET_ERR_CS_CACHE_NOT_INIT         = -41; // CS cache is not initialised (Read from chip / file required)
const int32 UET_ERR_CS_VERSION                = -42; // CS version error or incompatibility


const int32 UET_NVM_TYPE_EEPROM   = 1;
const int32 UET_NVM_TYPE_SPIFLASH = 2;

const int32 UET_CS_MERGE_CS_ONLY       = 0;
const int32 UET_CS_MERGE_FIRMWARE_ONLY = 1;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*************************************************************************************

    Function :      int32 uetOpen(const char*  connOpts, const char* csKeyFile, 
                                  CsrHandle_t* handle)

    Parameters :    connOpts - 
                        String defining the connection options. A NULL string results
                        in the default transport options being used, as may be set
                        using environment variables. If this parameter is NULL, and
                        there are no environment variables set, the transport will
                        default to use LPT1 on Windows, and the first enumerated USB-SPI
                        device on Linux. 
                        <p>The string may be one of those returned by 
                        ptutGetTransports, which returns transport strings for all 
                        available ports.
                        <p>Examples of options that can be listed in this
                        string (space separated) are:
                        <table>
                            <tr><td>SPITRANS=<trans>, where trans = LPT/USB
                            <tr><td>SPIPORT=<port>, where port is the port number
                            <tr><td>SPIMUL=<device>, where device is the zero-indexed 
                                    device, 0 - 15 on gang programmer hardware.
                        </table>

                    csKeyFile -
                        Full path to a CS KEY definition file (e.g. cskey_db.xml).
                        Set this parameter to NULL to use the default path.

                    handle -
                        Pointer to a location where the handle will be stored.
                        The value returned may be NULL if the internal engine object
                        could not be created (e.g. out of memory). Otherwise the
                        handle will be valid (not NULL), and must therefore be
                        closed using uetClose, even if the function returns an error.

    Returns :       Status of the command (error code). If handle is NULL returns 
                    UET_ERR_PARAM_INVALID.

    Description :   This function initialises the uEnergyTest DLL and opens a 
                    transport connection, returning a handle to the user.

    Example :

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=LPT SPIPORT=1", NULL, &handle);
        if(retVal == UET_OK)
        {
            // Perform testing here

            uetClose(handle);
        }
        else if(handle != 0)
        {
            cout << uetGetLastError(handle) << endl;
            uetClose(handle);
        }
        else
        {
            cout << "Error opening - no handle returned" << endl;
        }

*************************************************************************************/
UENERGYTEST_API(int32) uetOpen(const char*  connOpts, const char* csKeyFile, 
                               CsrHandle_t* handle);

/*************************************************************************************

    Function :      int32 uetClose(CsrHandle_t handle)

    Parameters :    handle - 
                        Device connection handle.

    Returns :       Status of the command (error code).

    Description :   This function terminates a connection with the DUT (established
                    using uetOpen).

    Example :       See example code for uetOpen.

*************************************************************************************/
UENERGYTEST_API(int32) uetClose(CsrHandle_t handle);

/*************************************************************************************

    Function :      const char* uetGetLastError(CsrHandle_t handle)

    Parameters :    handle - 
                        Device connection handle.

    Returns :       The details of the last error.

    Description :   This function gets a textual description of the last error. A 
                    valid connection handle is required.

    Example :       See example code for uetOpen.

*************************************************************************************/
UENERGYTEST_API(const char*) uetGetLastError(CsrHandle_t handle);

/*************************************************************************************
 
    Function :      int32 uetGetTransports(uint16* maxLen, char* ports, char* trans, 
                                           uint16* count)

    Parameters :    maxLen - 
                        Size of the arrays pointed to by the ports and trans 
                        parameters. If this parameter indicates that the ports or 
                        trans arrays are too small to store the complete strings, 
                        then the value is set to the size required (and error is 
                        returned).
                        If any other error occurs, this value is set to zero.

                    ports -
                        Pointer to an array of ASCII chars where the comma separated
                        list of available port names will be stored. These are
                        readable names which could be used for user selection.

                    trans -
                        Pointer to an array of ASCII chars where the comma separated
                        list of transport options for each of the available ports
                        will be stored. The transport options for a port can be 
                        passed directly into the uetOpen function to open the port.

                    count -
                        This value is set to the number of available ports found.

    Returns :       Status of the command (error code).

    Description :   This function is used to get a list of available SPI ports. A 
                    char array, pointed to by the ports parameter, is filled with 
                    a comma separated list of port names. A further char array, 
                    pointed to by the trans parameter, is filled with a comma 
                    separated list of the relevant transport option strings that
                    specify each available SPI port.
                    <p>If the maxLen parameter indicates that either char array is not
                    large enough to contain the strings, Error is returned and the
                    maxLen parameter is set to the size required for the arrays.
                    <p>If any other error occurs, the maxLen parameter is set to 0.
                    <p>This function can be used by an application to get a list
                    of available SPI ports with which to populate a drop down list
                    or other means of selection. The strings returned in the ports
                    parameter are in human readable format for display / selection.
                    The strings returned in the trans parameter can be passed directly
                    to the uetOpen function to open the port.

    Example :

        uint16 maxLen(256);
        uint16 count(0);
        char* portsStr = new char[maxLen];
        char* transStr = new char[maxLen];
        vector<string> ports; // The human readable port strings (e.g. "LPT1")
        vector<string> trans; // The transport option strings (e.g. "SPITRANS=LPT SPIPORT=1")

        int32 status = uetGetTransports(&maxLen, portsStr, transStr, &count);
        if( status != UET_OK && maxLen != 0 )
        {
            // Not enough space - resize the storage
            delete[] portsStr;
            portsStr = new char[maxLen];
            delete[] transStr;
            transStr = new char[maxLen];
            status = uetGetTransports(&maxLen, portsStr, transStr, &count);
        }
        if( status != UET_OK || count == 0 )
        {
            cout << "Error getting SPI ports, or none found" << endl;
            delete[] portsStr;
            delete[] transStr;
            return;
        }

        // Split up the comma separated strings of ports / transport options
        split(ports, portsStr, ','); // Use these for user selection (e.g. drop down list)
        split(trans, transStr, ','); // Use these to open a transport

        // Open the SPI port using the trans string
        // For the purposes of this example, we're just using the first in the list
        CsrHandle_t handle(0);
        status = uetOpen(trans.at(0).c_str(), NULL, &handle);

        if(status == UET_OK)
        {
            cout << "Connected!" << endl;

            // Perform all your testing here

            uetClose(handle);
        }
        else if(handle != 0)
        {
            uetClose(handle);
        }

        delete[] portsStr;
        delete[] transStr;

*************************************************************************************/
UENERGYTEST_API(int32) uetGetTransports(uint16* maxLen, char* ports, char* trans, 
                                        uint16* count);

/*************************************************************************************

    Function :      int32 uetLoadPtFirmware(CsrHandle_t handle, const char* filePath,
                                            uint8 useCsCache)

    Parameters :    handle - 
                        Device connection handle.

                    filePath -
                        String defining the full path of the ptest firmware XBV file 
                        to load.

                    useCsCache -
                        Boolean value that specifies whether the default CS settings
                        in the ptest firmware or the CS settings from the current CS
                        cache will be used.
                        <table>
                            <tr><td>0   <td>Use ptest firmware default CS
                            <tr><td>1   <td>Use the current CS cache
                        </table>
                        Values greater than 1 have the same effect as the value 1.

    Returns :       Status of the command (error code).

    Description :   This function resets the device, loads the specified ptest 
                    firmware image to RAM and restarts the processor. This function 
                    must be called before any hardware test functions.

    Example :       See example code for uetGetPtFwBuildInfo.

*************************************************************************************/
UENERGYTEST_API(int32) uetLoadPtFirmware(CsrHandle_t handle, const char* filePath,
                                         uint8 useCsCache);

/*************************************************************************************

    Function :      int32 uetGetVersion(uint8* major, uint8* minor, uint8* release, 
                                        uint8* build)

    Parameters :    major - 
                        Location to store the major version number.

                    minor -
                        Location to store the minor version number.

                    release -
                        Location to store the release number.

                    build -
                        Location to store the build number.

    Returns :       Status of the command (error code).

    Description :   This function gets the version information of the uEnergyTest DLL.
                    A connection is not required for this function.

*************************************************************************************/
UENERGYTEST_API(int32) uetGetVersion(uint8* major, uint8* minor, uint8* release, 
                                     uint8* build);

/*************************************************************************************

    Function :      int32 uetGetPtFwBuildInfo(CsrHandle_t handle, 
                                              const char* filePath, uint16* maxLen, 
                                              char* buildInfo)

    Parameters :    handle - 
                        Device connection handle.

                    filePath -
                        Path of an XBV file from which to obtain the build information.
                        Set this to NULL to obtain the build information of previously 
                        loaded firmware.

                    maxLen -
                        Size of the array pointed to by the buildInfo parameter. If 
                        this parameter indicates that the array is too small to store 
                        the complete string, then the value is set to the size 
                        required (and an error is returned). If any other error 
                        occurs, this value is set to zero.

                    buildInfo -
                        Location to store the build information string. The buffer 
                        size should be maxLen bytes.

    Returns :       Status of the command (error code).

    Description :   This function gets the build information string from a specified
                    XBV file or previously loaded firmware.
                    <p>The build information string contains version information and 
                    also build date and time. Each ptest firmware release has a unique 
                    build information string.

    Example :

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            static const uint16 INITIAL_BUFFER_LEN = 256;
            uint16 maxLen = INITIAL_BUFFER_LEN;
            char* buildInfo = new char[maxLen];
            retVal = uetLoadPtFirmware(handle, "ptest_CSR101x.xbv", 0);
            if(retVal == UET_OK)
            {
                // Get the firmware information for the currently running firmware
                retVal = uetGetPtFwBuildInfo(handle, NULL, &maxLen, buildInfo);
                if(retVal != UET_OK && maxLen != 0)
                {
                    delete[] buildInfo;
                    buildInfo = new char[maxLen];
                    retVal = uetGetPtFwBuildInfo(handle, NULL, &maxLen, buildInfo);
                }

                if(retVal == UET_OK)
                {
                    cout << "Build Info = " << buildInfo << endl;
                }
            }

            delete[] buildInfo;

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetGetPtFwBuildInfo(CsrHandle_t handle, const char* filePath, 
                                           uint16* maxLen, char* buildInfo);

/*************************************************************************************

    Function :      int32 uetGetPtFwBuildId(CsrHandle_t handle, const char* filePath, 
                                            uint16* buildId)

    Parameters :    handle - 
                        Device connection handle.

                    filePath -
                        Path of an XBV file from which to obtain the firmware build 
                        ID. Set this to NULL to obtain the build ID of the previously 
                        loaded firmware.

                    buildId -
                        Location to store the firmware's build ID.

    Returns :       Status of the command (error code).

    Description :   This function gets the ptest firmware build ID from a specified 
                    XBV file or previously loaded ptest firmware.
                    <p>Each ptest firmware release has a unique build ID.

    Example :

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            uint16 buildId;
            // Get the ID from the ptest firmware file (without loading it)
            retVal = uetGetPtFwBuildId(handle, "ptest_CSR101x.xbv", &buildId);
            if(retVal == UET_OK)
            {
                cout << "Build ID = " << buildId << endl;
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetGetPtFwBuildId(CsrHandle_t handle, const char* filePath, 
                                         uint16* buildId);

/*************************************************************************************

    Function :      int32 uetGetPtFwCsVersion(CsrHandle_t handle,  
                                              const char* filePath, uint16* csVersion)

    Parameters :    handle - 
                        Device connection handle.

                    filePath -
                        Path of an XBV file from which to obtain the CS version.
                        Set this to NULL to obtain the CS version of the previously 
                        loaded firmware.

                    csVersion -
                        Location to store the CS version.

    Returns :       Status of the command (error code).

    Description :   This function gets the Configuration Store (CS) version from a 
                    specified .xbv file or previously loaded ptest firmware.
                    <p>The CS version indicates which version of the CS the ptest 
                    firmware was built with, and can be used to check CS version 
                    compatibility. The CS version changes only when the CS elements 
                    change, therefore multiple ptest firmware releases may have the 
                    same CS version.

    Example :

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            uint16 csVersion;
            // Get the CS version from the ptest firmware file (without loading it)
            retVal = uetGetPtFwCsVersion(handle, "ptest_CSR101x.xbv", &csVersion);
            if(retVal == UET_OK)
            {
                cout << "CS Version = " << csVersion << endl;
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetGetPtFwCsVersion(CsrHandle_t handle, const char* filePath, 
                                           uint16* csVersion);

/*************************************************************************************

    Function :      int32 uetGetPtFwApiVersion(CsrHandle_t handle, uint32* version)

    Parameters :    handle - 
                        Device connection handle.

                    version -
                        Location to store the firmware's API version.

    Returns :       Status of the command (error code).

    Description :   This function gets the ptest firmware API version number from the 
                    previously loaded ptest firmware.
                    <p>The ptest firmware API version changes only when the API to the 
                    ptest firmware changes, e.g. when new features are added. It can 
                    be used by applications to check for feature support.

*************************************************************************************/
UENERGYTEST_API(int32) uetGetPtFwApiVersion(CsrHandle_t handle, uint32* version);

/*************************************************************************************

    Function :      int32 uetGetChipId(CsrHandle_t handle, uint16* chipId)

    Parameters :    handle - 
                        Device connection handle.

                    chipId -
                        Location to store the chip id value.

    Returns :       Status of the command (error code).

    Description :   This function gets the chip ID of a connected device.

    Example :

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            uint16 chipId;
            retVal = uetGetChipId(handle, &chipId);
            if(retVal == UET_OK)
            {
                cout << "Chip ID = " << chipId << endl;
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetGetChipId(CsrHandle_t handle, uint16* chipId);

/*************************************************************************************

    Function :      int32 uetRadSetXtalTrim(CsrHandle_t handle, uint16 value)

    Parameters :    handle - 
                        Device connection handle.

                    value -
                        The value to set (0...63).

    Returns :       Status of the command (error code).

    Description :   This function sets a new crystal trim value (that currently in use 
                    by the ptest firmware). To store the value in the CS, use 
                    uetCsCacheWriteItem.

    Example :

        static const uint8 XTAL_TRIM_CHANNEL = 19;
        static const uint16 INITIAL_XTAL_TRIM_VALUE = 32;

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            retVal = uetLoadPtFirmware(handle, "ptest_CSR101x.xbv", 0);
            if(retVal == UET_OK)
            {
                // Setting initial crystal trim value
                uint16 trimVal = INITIAL_XTAL_TRIM_VALUE;
                retVal = uetRadSetXtalTrim(handle, trimVal);
                if(retVal == UET_OK)
                {
                    retVal = uetRadCwStart(handle, XTAL_TRIM_CHANNEL, 0);
                }

                if(retVal == UET_OK)
                {
                    // Measure the frequency / frequency offset

                    // adjust the crystal trim value using an appropriate search algorithm
                    // e.g. binary search
                    retVal = uetRadSetXtalTrim(handle, trimVal);
                }

                // Measure frequency / frequency offset

                // Adjust and measure until frequency offset is within acceptable limits

                // Set the crystal trim CS value (See uetCsCacheWriteItem)

                // Stop CW
                if(retVal == UET_OK)
                {
                    retVal = uetRadStop(handle);
                }
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetRadSetXtalTrim(CsrHandle_t handle, uint16 value);

/*************************************************************************************

    Function :      int32 uetRadCwStart(CsrHandle_t handle, uint8 channel, 
                                        uint16 freqMhz)

    Parameters :    handle - 
                        Device connection handle.

                    channel -
                        Channel to transmit on (0...39).

                    freqMhz -
                        Frequency override value. If 0, the frequency is calculated 
                        from the channel value. Otherwise the valid range is 
                        2402...2480 (MHz).

    Returns :       Status of the command (error code).

    Description :   This function configures and starts CW Sine transmission.

    Example :       See example code for uetRadSetXtalTrim.

*************************************************************************************/
UENERGYTEST_API(int32) uetRadCwStart(CsrHandle_t handle, uint8 channel, 
                                     uint16 freqMhz);

/*************************************************************************************

    Function :      int32 uetRadStop(CsrHandle_t handle)

    Parameters :    handle - 
                        Device connection handle.

    Returns :       Status of the command (error code).

    Description :   This function stops the current radio transmission / reception 
                    mode. It can be used to stop transmission started with 
                    uetRadCwStart or uetRadTxStart, and reception started with 
                    uetRadRxStart.

    Example :       See example code for uetRadSetXtalTrim.

*************************************************************************************/
UENERGYTEST_API(int32) uetRadStop(CsrHandle_t handle);

/*************************************************************************************

    Function :      int32 uetRadTxStart(CsrHandle_t handle, uint8 channel, 
                                        uint8 txLength, uint8 payload, 
                                        uint16 numPackets)

    Parameters :    handle - 
                        Device connection handle.

                    channel -
                        Channel to transmit on (0...39).

                    txLength -
                        The length in bytes (0...37) of the payload data in each 
                        packet.

                    payload -
                        The transmit packet payload, where valid values are:
                        <table>
                            <tr><td>0   <td>Pseudo-Random bit sequence 9
                            <tr><td>1   <td>Pattern of alternating bits "11110000"
                            <tr><td>2   <td>Pattern of alternating bits "10101010"
                            <tr><td>3   <td>Pseudo-Random bit sequence 15
                            <tr><td>4   <td>Pattern of all "1" bits
                            <tr><td>5   <td>Pattern of all "0" bits
                            <tr><td>6   <td>Pattern of alternating bits "00001111"
                            <tr><td>7   <td>Pattern of alternating bits "01010101"
                        </table>

                    numPackets -
                        The number of packets to transmit. If 0, packets are 
                        continuously transmitted. If > 0, the given number of packets 
                        will be transmitted, after which transmission will stop 
                        automatically. In this case, uetRadTxGetPkts can be used to 
                        get the number of packets left to send.

    Returns :       Status of the command (error code).

    Description :   This function starts packet transmission.

    Example :

        static const uint8 TEST_CHANNEL = 19;
        static const uint8 TEST_PAYLOAD_LEN = 37;
        static const uint8 TEST_PAYLOAD_TYPE = 0; // PRBS9
        static const uint16 TEST_NUM_PACKETS = 0; // Continuous

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            retVal = uetLoadPtFirmware(handle, "ptest_CSR101x.xbv", 0);
            if(retVal == UET_OK)
            {
                retVal = uetRadTxStart(handle, TEST_CHANNEL, TEST_PAYLOAD_LEN, 
                                       TEST_PAYLOAD_TYPE, TEST_NUM_PACKETS);
                if(retVal == UET_OK)
                {
                    // Take TX measurements here

                    retVal = uetRadStop(handle);
                }
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetRadTxStart(CsrHandle_t handle, uint8 channel, 
                                     uint8 txLength, uint8 payload, 
                                     uint16 numPackets);

/*************************************************************************************

    Function :      int32 uetRadTxSetPowerLevel(CsrHandle_t handle, uint8 powerLevel)

    Parameters :    handle - 
                        Device connection handle.

                    powerLevel -
                        TX power level (0...7), where power level 0 gives the lowest 
                        output power.

    Returns :       Status of the command (error code).

    Description :   This function sets the power level to be used for transmissions. 
                    For packet transmission started with uetRadTxStart, the power 
                    level can be changed both before and during transmission. For CW 
                    transmission started with uetRadCwStart, setting the power level 
                    during transmission causes CW transmission to stop (requiring 
                    uetRadCwStart to be called again to restart CW output).
                    <p>The value set will override the value set via the 
                    tx_power_level CS key.

    Example :

        static const uint8 TEST_CHANNEL = 19;
        static const uint8 TEST_PAYLOAD_LEN = 37;
        static const uint8 TEST_PAYLOAD_TYPE = 0; // PRBS9
        static const uint16 TEST_NUM_PACKETS = 0; // Continuous
        static const uint8 TEST_POWER_LEVEL = 7;

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            retVal = uetLoadPtFirmware(handle, "ptest_CSR101x.xbv", 0);
            if(retVal == UET_OK)
            {
                retVal = uetRadTxSetPowerLevel(handle, TEST_POWER_LEVEL);
                if(retVal == UET_OK)
                {
                    retVal = uetRadTxStart(handle, TEST_CHANNEL, TEST_PAYLOAD_LEN, 
                                           TEST_PAYLOAD_TYPE, TEST_NUM_PACKETS);
                }

                if(retVal == UET_OK)
                {
                    // Take TX measurements here

                    retVal = uetRadStop(handle);
                }
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetRadTxSetPowerLevel(CsrHandle_t handle, uint8 powerLevel);

/*************************************************************************************

    Function :      int32 uetRadTxGetPkts(CsrHandle_t handle, uint16* numPackets)

    Parameters :    handle - 
                        Device connection handle.

                    numPackets -
                        Location to store the number of packets left to send.

    Returns :       Status of the command (error code).

    Description :   This function gets the number of packets left to send (for use 
                    after uetRadTxStart has been called to transmit a fixed number of 
                    packets).
                    <p>If the number of packets is 0, either all packets have been 
                    sent, or a packet limit was not used.

    Example :

        static const uint8 TEST_CHANNEL = 19;
        static const uint8 TEST_PAYLOAD_LEN = 37;
        static const uint8 TEST_PAYLOAD_TYPE = 0; // PRBS9
        static const uint16 TEST_NUM_PACKETS = 10000;
        static const uint16 PKT_SEND_POLL_INTERVAL_MS = 1000;

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            retVal = uetLoadPtFirmware(handle, "ptest_CSR101x.xbv", 0);
            if(retVal == UET_OK)
            {
                retVal = uetRadTxStart(handle, TEST_CHANNEL, TEST_PAYLOAD_LEN, 
                                       TEST_PAYLOAD_TYPE, TEST_NUM_PACKETS);
                if(retVal == UET_OK)
                {
                    // Wait for all packets to be sent
                    uint16 pktsRemaining;
                    do
                    {
                        retVal = uetRadTxGetPkts(handle, &pktsRemaining);
                        if(pktsRemaining > 0)
                        {
                            Sleep(PKT_SEND_POLL_INTERVAL_MS);
                        }
                    }
                    while(retVal == UET_OK && pktsRemaining != 0);

                    // Get number of received packets for PER calculation

                    retVal = uetRadStop(handle);
                }
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetRadTxGetPkts(CsrHandle_t handle, uint16* numPackets);

/*************************************************************************************

    Function :      int32 uetRadRxStart(CsrHandle_t handle, uint8 channel)

    Parameters :    handle - 
                        Device connection handle.

                    channel -
                        Channel to transmit on (0...39).

    Returns :       Status of the command (error code).

    Description :   This function configures the DUT to start receiving packets.

    Example :

        static const uint8 TEST_CHANNEL = 19;

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            retVal = uetLoadPtFirmware(handle, "ptest_CSR101x.xbv", 0);
            if(retVal == UET_OK)
            {
                retVal = uetRadRxStart(handle, TEST_CHANNEL);
                if(retVal == UET_OK)
                {
                    // Start test equipment packet transmission here

                    // Sleep for the required test duration (for the required 
                    // number of packets to be sent)
                    Sleep(1000);

                    uint16 numPackets;
                    int16 rssiAverage;
                    int32 unused;
                    retVal = uetRadRxGetResults(handle, &numPackets, &rssiAverage, &unused);
                    if(retVal == UET_OK)
                    {
                        cout << "Packets received = " << numPackets << endl;

                        // Calculate PER from expected and received packets values

                        if(numPackets > 0)
                        {
                            cout << "Approximate RSSI (average) = " << rssiAverage << " dBm" << endl;
                        }
                    }

                    retVal = uetRadStop(handle);
                }
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetRadRxStart(CsrHandle_t handle, uint8 channel);

/*************************************************************************************

    Function :      int32 uetRadRxGetPkts(CsrHandle_t handle, uint16* numPackets)

    Parameters :    handle - 
                        Device connection handle.

                    numPackets -
                        Location to store the number of packets received.

    Returns :       Status of the command (error code).

    Description :   This function retrieves the number of packets received value from 
                    the DUT.
                    <p>NOTE: This function has been deprecated. Use 
                    uetRadRxGetResults to get the number of received packets with 
                    other RX results.

    Deprecated :

*************************************************************************************/
UENERGYTEST_API(int32) uetRadRxGetPkts(CsrHandle_t handle, uint16* numPackets);

/*************************************************************************************

    Function :      int32 uetRadRxGetResults(CsrHandle_t handle, uint16* numPackets,
                                             int16* rssiAverage, int32* reserved)

    Parameters :    handle - 
                        Device connection handle.

                    numPackets -
                        Location to store the number of packets received.

                    rssiAverage -
                        Location to store the approximate Received Signal Strength 
                        Indication (RSSI) in dBm, averaged over the number of received 
                        packets. This value is valid only when numPackets is greater 
                        than 0.

                    reserved -
                        Reserved parameter (currently unused).

    Returns :       Status of the command (error code).

    Description :   This function retrieves the RX results from the DUT.

    Example :       See example code for uetRadRxStart.

*************************************************************************************/
UENERGYTEST_API(int32) uetRadRxGetResults(CsrHandle_t handle, uint16* numPackets, 
                                          int16* rssiAverage, int32* reserved);

/*************************************************************************************

    Function :      int32 uetPioAssign(CsrHandle_t handle, uint32 mask, 
                                       uint32 direction)

    Parameters :    handle - 
                        Device connection handle.

                    mask -
                        The bits of this parameter each represent one of 32 possible 
                        PIO lines (the number of PIOs available depends on the IC 
                        type). If a bit is set, the direction of the PIO will be set 
                        according to the direction parameter. If a bit is unset, the 
                        PIO will not be affected by this function.

                    direction -
                        The bits of this parameter each represent one of 32 possible 
                        PIO lines( the number of PIOs available depends on the IC 
                        type). Only bits set in the mask will take effect. If a bit 
                        is set, the PIO is configured as an output, otherwise as an 
                        input.

    Returns :       Status of the command (error code).

    Description :   This function can be used to assign selected PIO lines of the DUT 
                    as inputs / outputs.

    Example :       See example code for uetPioSet.

*************************************************************************************/
UENERGYTEST_API(int32) uetPioAssign(CsrHandle_t handle, uint32 mask, 
                                    uint32 direction);

/*************************************************************************************

    Function :      int32 uetPioGet(CsrHandle_t handle, uint32* direction, 
                                    uint32* state)

    Parameters :    handle - 
                        Device connection handle.

                    direction -
                        Location where the value indicating the direction of up to 32 
                        PIO lines will be stored (the number of PIOs available depends 
                        on the IC type). If a bit is set, the PIO is configured as an 
                        output, otherwise as an input.

                    state -
                        Location where the state of each line will be stored (the 
                        actual number of PIOs available depends on the IC type).

    Returns :       Status of the command (error code).

    Description :   This function can be used to get the get the state of the PIO 
                    lines that have been assigned as inputs.

    Example :

        static const uint32 TEST_PIO_MASK = 0xFFFFF000; // 12-31
        static const uint32 TEST_PIO_DIRECTION_MASK = 0x00000000; // All inputs

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            retVal = uetLoadPtFirmware(handle, "ptest_CSR101x.xbv", 0);
            if(retVal == UET_OK)
            {
                retVal = uetPioAssign(handle, TEST_PIO_MASK, TEST_PIO_DIRECTION_MASK);
                if(retVal == UET_OK)
                {
                    uint32 direction, state;
                    retVal = uetPioGet(handle, &direction, &state);
                    if(retVal == UET_OK)
                    {
                        cout << "Direction = " << direction
                             << ", state = " << state << endl;
                    }
                }
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetPioGet(CsrHandle_t handle, uint32* direction, 
                                 uint32* state);

/*************************************************************************************

    Function :      int32 uetPioSet(CsrHandle_t handle, uint32 outputMask, 
                                    uint32 state)

    Parameters :    handle - 
                        Device connection handle.

                    outputMask -
                        Value indicating which of up to 32 PIO lines have been 
                        configured as outputs using uetPioAssign.

                    state -
                        The state to set for the PIOs configured as outputs. Only 
                        those PIOs where the corresponding bit in outputMask is set 
                        are affected.

    Returns :       Status of the command (error code).

    Description :   This function can be used to set the state of the PIO lines
                    that have been assigned as outputs.

    Example :

        static const uint32 TEST_PIO_MASK = 0xFFFFF000; // 12-31
        static const uint32 TEST_PIO_DIRECTION_MASK = 0xFFFFF000; // 12-31 as outputs
        static const uint32 TEST_PIO_STATE = 0xAAAAA000; // 1010 pattern

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            retVal = uetLoadPtFirmware(handle, "ptest_CSR101x.xbv", 0);
            if(retVal == UET_OK)
            {
                retVal = uetPioAssign(handle, TEST_PIO_MASK, TEST_PIO_DIRECTION_MASK);
                if(retVal == UET_OK)
                {
                    retVal = uetPioSet(handle, TEST_PIO_DIRECTION_MASK, TEST_PIO_STATE);
                }
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetPioSet(CsrHandle_t handle, uint32 outputMask, uint32 state);

/*************************************************************************************

    Function :      int32 uetUartLbTest(CsrHandle_t handle, const char* portName, 
                                        uint32 baudRate)

    Parameters :    handle - 
                        Device connection handle.

                    portName -
                        Name of the UART port to use for the test, e.g. "COM1".

                    baudRate -
                        UART baud rate to be used. Currently supported rates in the 
                        firmware are (rates supported by serial port hardware may be 
                        different):
                        <table>
                            <tr><td>2400
                            <tr><td>9600
                            <tr><td>19200
                            <tr><td>38400
                            <tr><td>57600
                            <tr><td>115200
                            <tr><td>230400
                            <tr><td>460800
                            <tr><td>921600
                            <tr><td>1382400
                            <tr><td>1843200
                            <tr><td>2764800
                            <tr><td>3686400
                            <tr><td>5529600
                        </table>

    Returns :       Status of the command (error code).

    Description :   This function can be used to test the UART lines using a loopback 
                    test. The function first commands the DUT into a UART loopback 
                    mode, then opens a UART connection, writes some data, then reads 
                    from the device. If the data matches, the test passes, otherwise 
                    an error code indicating that the test failed is returned. UART 
                    loopback mode is switched off after the test has been performed.

    Example :

        static const std::string PORT = "COM1";
        static const uint32 BAUDRATE = 9600;

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            retVal = uetLoadPtFirmware(handle, "ptest_CSR101x.xbv", 0);
            if(retVal == UET_OK)
            {
                retVal = uetUartLbTest(handle, PORT.c_str(), BAUDRATE);
                if(retVal == UET_OK)
                {
                    cout << "UART Loopback test PASSED" << endl;
                }
                else if(retVal == UET_ERR_TEST_FAILED)
                {
                    cout << "UART Loopback test FAILED" << endl;
                }
                else
                {
                    cout << "ERROR during UART test: " << uetGetLastError(handle) << endl;
                }
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetUartLbTest(CsrHandle_t handle, const char* portName, 
                                     uint32 baudRate);

/*************************************************************************************

    Function :      int32 uetLedSet(CsrHandle_t handle, uint32 mask, uint8 mode, 
                                    uint16 intervalMs, uint8 brightness)

    Parameters :    handle - 
                        Device connection handle.

                    mask -
                        Mask used to indicate which of up to 32 PIOs, used to drive 
                        LEDs, are to be routed through the PWM hardware and set to the 
                        given mode. The number of PIOs supported and available for use 
                        to drive LEDs depends on the IC / package.

                    mode -
                        The LED mode to set, where valid values are:
                        <table>
                            <tr><td>0 = Off
                            <tr><td>1 = On
                            <tr><td>2 = Flash
                            <tr><td>3 = Ramp
                        </table>

                    intervalMs -
                        The interval, in milliseconds, used when the mode is Flash or 
                        Ramp. In the case of Flash mode, the interval is the on/off 
                        time. In the case of Ramp mode, the interval is the time taken 
                        to ramp up/down.

                    brightness -
                        The brightness, in percent, used when the mode is anything 
                        other than Off. In the case of Flash mode, it is the 
                        brightness set for the on state. In the case of Ramp mode, it 
                        is the maximum brightness.

    Returns :       Status of the command (error code).

    Description :   This function can be used to control LED modes.

    Example :

        static const uint32 TEST_LED_MASK = 0x0000F000; // 12-15
        static const uint8 LED_MODE_OFF = 0;
        static const uint8 LED_MODE_FLASH = 2;
        static const uint16 INTERVAL_MS = 250;
        static const uint16 BRIGHTNESS_PCT = 100;

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            retVal = uetLoadPtFirmware(handle, "ptest_CSR101x.xbv", 0);
            if(retVal == UET_OK)
            {
                retVal = uetLedSet(handle, TEST_LED_MASK, LED_MODE_FLASH, INTERVAL_MS, BRIGHTNESS_PCT);

                if(retVal == UET_OK)
                {
                    // Visual or other check of LEDs here

                    retVal = uetLedSet(handle, TEST_LED_MASK, LED_MODE_OFF, 0, 0);
                }
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetLedSet(CsrHandle_t handle, uint32 mask, uint8 mode, 
                                 uint16 intervalMs, uint8 brightness);

/*************************************************************************************

    Function :      int32 uetAioSet(CsrHandle_t handle, uint8 aio, uint16 voltageMv)

    Parameters :    handle - 
                        Device connection handle.

                    aio -
                        The AIO line to set. The number of AIO lines available 
                        depends on the IC / package.

                    voltageMv -
                        The voltage, in millivolts to set for the AIO line. If the 
                        voltage requested is above the maximum, the AIO line is set 
                        to the maximum voltage. See the IC datasheet for the maximum 
                        AIO voltage.

    Returns :       Status of the command (error code).

    Description :   This function can be used to configure an AIO line as an output 
                    and set the output voltage.

    Example :

        static const uint8 TEST_AIO = 0;
        static const uint16 TEST_VOLTAGE_MV = 600;

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            retVal = uetLoadPtFirmware(handle, "ptest_CSR101x.xbv", 0);
            if(retVal == UET_OK)
            {
                retVal = uetAioSet(handle, TEST_AIO, TEST_VOLTAGE_MV);
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetAioSet(CsrHandle_t handle, uint8 aio, uint16 voltageMv);

/*************************************************************************************

    Function :      int32 uetAioGet(CsrHandle_t handle, uint8 aio, uint16* voltageMv)

    Parameters :    handle - 
                        Device connection handle.

                    aio -
                        The AIO line to read. The number of AIO lines available 
                        depends on the IC / package.

                    voltageMv -
                        Location to store the voltage (in millivolts).

    Returns :       Status of the command (error code).

    Description :   This function can be used to configure an AIO line as an input
                    and get the voltage.

    Example :

        static const uint8 TEST_AIO = 0;

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            retVal = uetLoadPtFirmware(handle, "ptest_CSR101x.xbv", 0);
            if(retVal == UET_OK)
            {
                uint16 voltageMv;
                retVal = uetAioGet(handle, TEST_AIO, &voltageMv);
                if(retVal == UET_OK)
                {
                    cout << "AIO voltage (mV) = " << voltageMv << endl;
                }
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetAioGet(CsrHandle_t handle, uint8 aio, uint16* voltageMv);

/*************************************************************************************

    Function :      int32 uetCsCacheWriteItem(CsrHandle_t handle, const char* name, 
                                              uint16 value[], uint16* length);

    Parameters :    handle - 
                        Device connection handle.

                    name -
                        The name of the CS item to write.

                    value -
                        The value to write for the CS item. This is a pointer to an 
                        array of uint16 values (a CS item’s value may be made up of 
                        1...n words).

                    length -
                        The length of the value in words. If the length of the value 
                        does not match that expected for the item, an error is 
                        returned, and this value is set to the expected length.

    Returns :       Status of the command (error code).

    Description :   Writes a configuration store value to the CS cache.

    Example :       See example code for uetCsCacheReadFromFile.

*************************************************************************************/
UENERGYTEST_API(int32) uetCsCacheWriteItem(CsrHandle_t handle, const char* name, 
                                           uint16 value[], uint16* length);

/*************************************************************************************

    Function :      int32 uetCsCacheReadItem(CsrHandle_t handle, const char* name, 
                                             uint16 value[], uint16* length);

    Parameters :    handle - 
                        Device connection handle.

                    name -
                        The name of the CS item to read.

                    value -
                        Location where the CS key value will be stored. This is a 
                        pointer to an array of uint16 values (a CS item’s value may 
                        be made up of 1..n words).

                    length -
                        Input / output parameter, where the input value is the length 
                        of the value array in words, and the output value is the 
                        actual length (in words) of the key’s value. If the given 
                        length is insufficient to store the key’s value, an error is 
                        returned (and the value will be set to the required length).

    Returns :       Status of the command (error code).

    Description :   Reads the value of a configuration store item from the CS cache.

    Example :       See example code for uetCsCacheReadFromFile.

*************************************************************************************/
UENERGYTEST_API(int32) uetCsCacheReadItem(CsrHandle_t handle, const char* name, 
                                          uint16 value[], uint16* length);

/*************************************************************************************

    Function :      int32 uetCsCacheReadFromChip(CsrHandle_t handle, uint8 type, 
                                                 uint8 csOnly);

    Parameters :    handle - 
                        Device connection handle.

                    type -
                        The Non-Volatile Memory type to read from, where valid values
                        are:
                        <table>
                            <tr><td>1   <td>UET_NVM_TYPE_EEPROM
                            <tr><td>2   <td>UET_NVM_TYPE_SPIFLASH
                        </table>

                    csOnly - 
                       Boolean value that specifies whether the entire image will be 
                       read from NVM, or just the configuration settings:
                        <table>
                            <tr><td>0   <td>Read entire image from NVM
                            <tr><td>1   <td>Read CS only
                        </table>
                        Values greater than 1 have the same effect as the value 1.                        

    Returns :       Status of the command (error code).

    Description :   This function can be used to read the configuration store data 
                    from Non-Volatile Memory (EEPROM / SPI-Flash) into the CS cache. 
                    If the CS build version read is unknown (i.e. not specified in the 
                    XML database used), an error is returned.
                    <p>This function replaces any existing CS cache with the CS data 
                    read.
                    <p>In order to read from non-volatile memory, this function writes 
                    a program to RAM. This overwrites any existing ptest firmware, 
                    which means that the ptest firmware must be reloaded before any 
                    further hardware or RF test operations can be performed. Therefore 
                    it is most efficient to read from NVM before or after hardware 
                    tests.

    Example :

        static const string CRYSTAL_TRIM_CS_NAME = "crystal_ftrim";
        static const uint8 CS_ONLY_RW_VALUE = 1;

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            // Read into the cache from chip (CS only)
            retVal = uetCsCacheReadFromChip(handle, UET_NVM_TYPE_EEPROM, CS_ONLY_RW_VALUE);

            // Read a CS item
            uint16 value[10];
            uint16 length = sizeof(value) / sizeof(uint16);
            if(retVal == UET_OK)
            {
                retVal = uetCsCacheReadItem(handle, CRYSTAL_TRIM_CS_NAME.c_str(), value, &length);
                if(retVal == UET_OK)
                {
                    cout << "Key value = " << value[0] << ", length = " << length << endl;
                }
            }

            // Update a CS item
            if(retVal == UET_OK)
            {
                value[0] = value[0] + 1;
                retVal = uetCsCacheWriteItem(handle, CRYSTAL_TRIM_CS_NAME.c_str(), value, &length);
            }

            // Write the updates back to the chip
            if(retVal == UET_OK)
            {
                retVal = uetCsCacheWriteToChip(handle, UET_NVM_TYPE_EEPROM, CS_ONLY_RW_VALUE);
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetCsCacheReadFromChip(CsrHandle_t handle, uint8 type, 
                                              uint8 csOnly);

/*************************************************************************************

    Function :      int32 uetCsCacheReadFromFile(CsrHandle_t handle, const char* file);

    Parameters :    handle - 
                        Device connection handle.

                    file -
                        Path of file (xuv/img/keyr) containing the CS data. The file 
                        extension indicates the file format.

    Returns :       Status of the command (error code).

    Description :   This function can be used to read the configuration store data 
                    from a file into the CS cache. If the CS build version read is 
                    unknown (i.e. not specified in the XML database used), an error is 
                    returned.
                    <p>This function replaces any existing CS cache items with the CS 
                    items read. keyr format files can be incomplete, i.e. not 
                    specifying all CS items. In this case, any items not currently in 
                    the cache will be set to their default values.

    Example :

        static const string CRYSTAL_TRIM_CS_NAME = "crystal_ftrim";

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            // Read into the cache from a file
            retVal = uetCsCacheReadFromFile(handle, "image_in.xuv");

            // Read the CS version
            if(retVal == UET_OK)
            {
                uint16 csVer;
                retVal = uetCsCacheGetVersion(handle, &csVer);
                if(retVal == UET_OK)
                {
                    cout << "CS version = " << csVer << endl;
                }
            }

            // Load the firmware using the current CS cache (rather than the default
            // cache in the firmware file)
            if(retVal == UET_OK)
            {
                retVal = uetLoadPtFirmware(handle, "ptest_CSR101x.xbv", 1);
            }

            // Read a CS item
            uint16 value[10];
            uint16 length = sizeof(value) / sizeof(uint16);
            if(retVal == UET_OK)
            {
                retVal = uetCsCacheReadItem(handle, CRYSTAL_TRIM_CS_NAME.c_str(), value, &length);
                if(retVal == UET_OK)
                {
                    cout << "Key value = " << value[0] << ", length = " << length << endl;
                }
            }

            // Calculate the real xtrim value (external measurement required)

            // Update a CS item
            if(retVal == UET_OK)
            {
                value[0] = value[0] + 1;
                retVal = uetCsCacheWriteItem(handle, CRYSTAL_TRIM_CS_NAME.c_str(), value, &length);
            }

            // Write the updates to a file
            if(retVal == UET_OK)
            {
                retVal = uetCsCacheWriteToFile(handle, "image_out.xuv");
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetCsCacheReadFromFile(CsrHandle_t handle, const char* file);

/*************************************************************************************

    Function :      int32 uetCsCacheWriteToChip(CsrHandle_t handle, uint8 type, 
                                                uint8 csOnly);

    Parameters :    handle - 
                        Device connection handle.

                    type -
                        The Non-Volatile Memory type to write to, where valid values
                        are:
                        <table>
                            <tr><td>1   <td>UET_NVM_TYPE_EEPROM
                            <tr><td>2   <td>UET_NVM_TYPE_SPIFLASH
                        </table>

                    csOnly - 
                       Boolean value that specifies whether the entire image will be 
                       written to NVM, or just the configuration settings:
                        <table>
                            <tr><td>0   <td>Write entire image to NVM
                            <tr><td>1   <td>Write CS only
                        </table>
                        Values greater than 1 have the same effect as the value 1.                        

    Returns :       Status of the command (error code).

    Description :   This function can be used to write the configuration store data 
                    from the CS cache to Non Volatile Memory (EEPROM / SPI-Flash). If 
                    the CS version in the cache does not match the version used by the 
                    firmware on chip, an error is returned.
                    <p>In the case where a complete firmware image (containing 
                    firmware as well as CS data) has been read into the cache (either 
                    from the chip, or from an xuv/img file), the complete image will 
                    be written to the chip.
                    <p>In order to write to non-volatile memory, this function writes 
                    a program to RAM. This overwrites any existing ptest firmware, 
                    which means that the ptest firmware must be reloaded before any 
                    further hardware or RF test operations can be performed. Therefore 
                    it is most efficient to write to NVM after hardware tests.

    Example :       See example code for uetCsCacheReadFromChip.

*************************************************************************************/
UENERGYTEST_API(int32) uetCsCacheWriteToChip(CsrHandle_t handle, uint8 type, 
                                             uint8 csOnly);

/*************************************************************************************

    Function :      int32 uetCsCacheWriteToFile(CsrHandle_t handle, const char* file);

    Parameters :    handle - 
                        Device connection handle.

                    file -
                        Path of file (xuv/img/keyr) where the CS data will be written. 
                        The file extension indicates the file format.

    Returns :       Status of the command (error code).

    Description :   This function can be used to write the configuration store data in 
                    the CS cache to a file. 
                    <p>In the case where a complete firmware image (containing 
                    firmware as well as CS data) has been read into the cache (either 
                    from the chip, or from an xuv/img file), the complete image will 
                    be written to the output file (except in the case of the keyr 
                    format, which can only contain CS information).

    Example :       See example code for uetCsCacheReadFromFile.

*************************************************************************************/
UENERGYTEST_API(int32) uetCsCacheWriteToFile(CsrHandle_t handle, const char* file);

/*************************************************************************************

    Function :      int32 uetCsCacheGetVersion(CsrHandle_t handle, uint16* csVersion);

    Parameters :    handle - 
                        Device connection handle.

                    csVersion -
                        Location to store the CS version.

    Returns :       Status of the command (error code).

    Description :   This function gets the Configuration Store version number from 
                    the current CS cache. If the CS cache is uninitialised, an error 
                    is returned.

    Example :       See example code for uetCsCacheReadFromFile.

*************************************************************************************/
UENERGYTEST_API(int32) uetCsCacheGetVersion(CsrHandle_t handle, uint16* csVersion);

/*************************************************************************************

    Function :      int32 uetCsCacheFileMerge(CsrHandle_t handle, 
                                              const char* sourceFile,
                                              uint8 mergeType);

    Parameters :    handle - 
                        Device connection handle.

                    sourceFile -
                        Path of file (xuv/img/keyr) containing the CS, or CS and 
                        firmware data to merge into the current CS cache. The file 
                        extension indicates the file format.

                    mergeType -
                        Merge type, used to control what is merged into the current 
                        cache, where valid values are:
                        <table>
                            <tr><td>0   <td>UET_CS_MERGE_CS_ONLY
                            <tr><td>1   <td>UET_CS_MERGE_FIRMWARE_ONLY
                        </table>

    Returns :       Status of the command (error code).

    Description :   This function can be used to merge configuration store or firmware 
                    image data, from a file, into the CS cache. Because keyr format 
                    files do not contain any firmware data, if a keyr file is used to 
                    provide CS settings for use during testing, it is necessary to read 
                    in a firmware image file (xuv/img) to provide the firmware data 
                    prior to writing to chip NVM (in this case, use mergeType = 1). If 
                    the CS settings used during testing were read from NVM or a 
                    firmware image file (xuv/img), firmware data will be present in 
                    the CS cache, and the CS data can be overridden with settings from 
                    any file (in this case, use mergeType = 0).
                    <p>The CS or firmware data read from the sourceFile replaces the 
                    corresponding data in the current CS cache. For keyr files, the 
                    function will return an error if mergeType = 1. The function will 
                    also return an error if the CS versions (cache and file) do not 
                    match.
    
    Example :      

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            // Read into the cache from a file
            retVal = uetCsCacheReadFromFile(handle, "cstest.keyr");

            // Merge the CS data into a complete firmware image file
            if(retVal == UET_OK)
            {
                retVal = uetCsCacheFileMerge(handle, "image_in.xuv", UET_CS_MERGE_FIRMWARE_ONLY);
            }

            // Write complete image to chip NVM
            if(retVal == UET_OK)
            {
                retVal = uetCsCacheWriteToChip(handle, UET_NVM_TYPE_EEPROM, 0);
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetCsCacheFileMerge(CsrHandle_t handle, const char* sourceFile, 
                                           uint8 mergeType);

/*************************************************************************************

    Function :      int32 uetCsCacheOtauApp(CsrHandle_t handle, uint16 csrOtauVersion,
                                            uint16 appOffset);

    Parameters :    handle - 
                        Device connection handle.

                    csrOtauVersion -
                        The CSR OTA (Over The Air) Update version matching the DUT 
                        firmware and image files.

                    appOffset -
                        The OTA Update application offset in bytes. If set to non-zero,
                        the given value is used instead of the value obtained 
                        internally from csrOtauVersion. Must be set to zero to use 
                        csrOtauVersion to determine the offset, or to use the 
                        bootloader CS.

    Returns :       Status of the command (error code).

    Description :   OTA Update enabled combined firmware images contain both a 
                    bootloader and application image, each with their own CS settings. 
                    By default, uEnergyTest works with the bootloader CS settings (or 
                    application settings for non-OTA Update enabled images). This 
                    function can be used to switch CS operations to work with the 
                    application CS settings.
                    <p>If the csrOtauVersion and appOffset are set to 0, CS operations 
                    will work with the default CS (OTA Update bootloader or non-OTA 
                    Update application).
                    <p>NOTE: If the OTAU version is < 7, OTA Update enabled images are 
                    only compatible with devices using EEPROM. For OTAU v7 and later, 
                    both EEPROM and SPI-Flash devices are supported.

    Example :

        static const string CRYSTAL_TRIM_CS_NAME = "crystal_ftrim";

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            // Read into the cache from a file
            retVal = uetCsCacheReadFromFile(handle, "combined_ota_image.img");

            // Load the firmware using the current CS cache (rather than the default
            // cache in the firmware file)
            if(retVal == UET_OK)
            {
                retVal = uetLoadPtFirmware(handle, "ptest_CSR101x.xbv", 1);
            }

            // Read default crystal trim
            uint16 value[10];
            uint16 length = sizeof(value) / sizeof(uint16);
            if(retVal == UET_OK)
            {
                retVal = uetCsCacheReadItem(handle, CRYSTAL_TRIM_CS_NAME.c_str(), value, &length);
                if(retVal == UET_OK)
                {
                    cout << "Key value = " << value[0] << ", length = " << length << endl;
                }
            }

            // Calculate the real xtrim value (external measurement required)

            // Update crystal trim (bootloader CS)
            if(retVal == UET_OK)
            {
                value[0] = value[0] + 1;
                retVal = uetCsCacheWriteItem(handle, CRYSTAL_TRIM_CS_NAME.c_str(), value, &length);
            }

            // Switch to application CS (using CSR OTA Update version 5)
            if(retVal == UET_OK)
            {
                retVal = uetCsCacheOtauApp(handle, 5, 0);
            }

            // Update crystal trim (application CS)
            if(retVal == UET_OK)
            {
                retVal = uetCsCacheWriteItem(handle, CRYSTAL_TRIM_CS_NAME.c_str(), value, &length);
            }

            // Write the complete image to chip
            if(retVal == UET_OK)
            {
                retVal = uetCsCacheWriteToChip(handle, UET_NVM_TYPE_EEPROM, false);
            }

            // Optional reset (causes the application in the image written above to run)
            if(retVal == UET_OK)
            {
                retVal = uetReset(handle);
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetCsCacheOtauApp(CsrHandle_t handle, uint16 csrOtauVersion,
                                         uint32 appOffset);

/*************************************************************************************

    Function :      int32 uetNvmCustomWrite(CsrHandle_t handle, uint8 type, 
                                            uint16 offset, const uint16* data, 
                                            uint16 length);

    Parameters :    handle - 
                        Device connection handle.

                    type -
                        The Non-Volatile Memory type to write to, where valid values
                        are:
                        <table>
                            <tr><td>1   <td>UET_NVM_TYPE_EEPROM
                            <tr><td>2   <td>UET_NVM_TYPE_SPIFLASH
                        </table>

                    offset -
                        The offset in words from the start of the customer NVM area to 
                        write to.

                    data -
                        The data to write.

                    length -
                        The length, in words, of the data to write.

    Returns :       Status of the command (error code).

    Description :   This function writes a block of data to a specified offset in 
                    the customer Non-Volatile Memory area.
                    <p>This function reads CS keys from the CS cache in order to 
                    get the details of the customer NVM area. Therefore CS keys must 
                    have been loaded using uetCsCacheReadFromFile or 
                    uetCsCacheReadFromChip prior to using this function.
                    <p>In order to write to non-volatile memory, this function writes 
                    a program to RAM. This overwrites any existing ptest firmware, 
                    which means that the ptest firmware must be reloaded before any 
                    further hardware or RF test operations can be performed. Therefore 
                    it is most efficient to write to NVM before or after hardware 
                    tests.

    Example :

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            // Read CS
            retVal = uetCsCacheReadFromFile(handle, "cstest.keyr");

            // Write some data to the start of the customer NVM area
            if(retVal == UET_OK)
            {
                uint16 data[] = { 0, 1, 2, 3 };
                retVal = uetNvmCustomWrite(handle, UET_NVM_TYPE_EEPROM, 0,
                                           data, sizeof(data) / sizeof(data[0]));
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetNvmCustomWrite(CsrHandle_t handle, uint8 type, 
                                         uint16 offset, const uint16* data, 
                                         uint16 length);

/*************************************************************************************

    Function :      int32 uetNvmCustomRead(CsrHandle_t handle, uint8 type, 
                                           uint16 offset, uint16* data, uint16 length);

    Parameters :    handle - 
                        Device connection handle.

                    type -
                        The Non-Volatile Memory type to read from, where valid values
                        are:
                        <table>
                            <tr><td>1   <td>UET_NVM_TYPE_EEPROM
                            <tr><td>2   <td>UET_NVM_TYPE_SPIFLASH
                        </table>

                    offset -
                        The offset in words from the start of the customer NVM area to 
                        read from.

                    data -
                        The data read. Must point to an array large enough to hold the 
                        number of words specified by the length parameter.

                    length -
                        The number of words to read.

    Returns :       Status of the command (error code).

    Description :   This function reads a block of data from a specified offset in 
                    the customer Non-Volatile Memory area.
                    <p>This function reads CS keys from the CS cache in order to 
                    get the details of the customer NVM area. Therefore CS keys must 
                    have been loaded using uetCsCacheReadFromFile or 
                    uetCsCacheReadFromChip prior to using this function.
                    <p>In order to read from non-volatile memory, this function writes 
                    a program to RAM. This overwrites any existing ptest firmware, 
                    which means that the ptest firmware must be reloaded before any 
                    further hardware or RF test operations can be performed. Therefore 
                    it is most efficient to read from NVM before or after hardware 
                    tests.

    Example :

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            // Read CS
            retVal = uetCsCacheReadFromFile(handle, "cstest.keyr");

            // Read some data from the start of the customer NVM area
            uint16 data[4];
            if(retVal == UET_OK)
            {
                retVal = uetNvmCustomRead(handle, UET_NVM_TYPE_EEPROM, 0,
                                          data, sizeof(data) / sizeof(data[0]));
            }

            // Process the data

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetNvmCustomRead(CsrHandle_t handle, uint8 type, uint16 offset, 
                                        uint16* data, uint16 length);

/*************************************************************************************

    Function :      int32 uetNvmUnlock(CsrHandle_t handle, uint8 type, 
                                       uint32 startAddress, uint32 endAddress);

    Parameters :    handle - 
                        Device connection handle.

                    type -
                        The Non-Volatile Memory type to write to, where valid values
                        are:
                        <table>
                            <tr><td>1   <td>UET_NVM_TYPE_EEPROM
                            <tr><td>2   <td>UET_NVM_TYPE_SPIFLASH
                        </table>

                    startAddress -
                        The start word address of the NVM section to unlock.

                    endAddress -
                        The end word address of the NVM section to unlock (inclusive, 
                        i.e. this address is included in the unlocked region).

    Returns :       Status of the command (error code).

    Description :   This function unlocks a block of NVM for writing, allowing 
                    uetNvmWrite to write data within the specified area. Only one 
                    region can be unlocked at any time, i.e. calling this function 
                    twice will leave only the second region unlocked. This function 
                    does not affect the operation of uetNvmCustomWrite - it is only 
                    required prior to using uetNvmWrite.
                    <p><b>NOTE: Writing data directly to NVM is potentially dangerous,
                    as it can result in corruption of firmware present in NVM, which 
                    is protected with CRCs. This function should be used with caution, 
                    and only for addresses which are known to be safe to write.</b>

    Example :       See example code for uetNvmWrite.

*************************************************************************************/
UENERGYTEST_API(int32) uetNvmUnlock(CsrHandle_t handle, uint8 type, 
                                    uint32 startAddress, uint32 endAddress);

/*************************************************************************************

    Function :      int32 uetNvmWrite(CsrHandle_t handle, uint8 type, uint32 address, 
                                      const uint16* data, uint16 length);

    Parameters :    handle - 
                        Device connection handle.

                    type -
                        The Non-Volatile Memory type to write to, where valid values
                        are:
                        <table>
                            <tr><td>1   <td>UET_NVM_TYPE_EEPROM
                            <tr><td>2   <td>UET_NVM_TYPE_SPIFLASH
                        </table>

                    address -
                        The word address in NVM to write to.

                    data -
                        The data to write.

                    length -
                        The length, in words, of the data to write.

    Returns :       Status of the command (error code).

    Description :   This function writes a block of data to a specified address in 
                    Non-Volatile Memory.
                    <p><b>NOTE: Writing data directly to NVM is potentially dangerous,
                    as it can result in corruption of firmware present in NVM, which
                    is protected with CRCs. This function should be used with caution,
                    and only for addresses which are known to be safe to write.
                    uetNvmUnlock must be called first to unlock the region of NVM to be 
                    written. If CRC protected areas are to be written, the entire 
                    protected block should be read, modified, CRC updated, and then 
                    written back.</b>
                    <p>In order to write to non-volatile memory, this function writes 
                    a program to RAM. This overwrites any existing ptest firmware, 
                    which means that the ptest firmware must be reloaded before any 
                    further hardware or RF test operations can be performed. Therefore 
                    it is most efficient to write to NVM before or after hardware 
                    tests.

    Example :

        static const uint32 WRITE_ADDRESS = 0xF000;

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            const uint16 data[] = { 0, 1, 2, 3 };

            // Unlock the NVM region for writing
            retVal = uetNvmUnlock(handle, UET_NVM_TYPE_EEPROM, WRITE_ADDRESS, 
                                  WRITE_ADDRESS + (sizeof(data) / sizeof(data[0])) - 1);

            // Write the data
            if(retVal == UET_OK)
            {
                retVal = uetNvmWrite(handle, UET_NVM_TYPE_EEPROM, WRITE_ADDRESS,
                                     data, sizeof(data) / sizeof(data[0]));
            }

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetNvmWrite(CsrHandle_t handle, uint8 type, uint32 address, 
                                   const uint16* data, uint16 length);

/*************************************************************************************

    Function :      int32 uetNvmRead(CsrHandle_t handle, uint8 type, uint32 address, 
                                     uint16* data, uint16 length);

    Parameters :    handle - 
                        Device connection handle.

                    type -
                        The Non-Volatile Memory type to read from, where valid values
                        are:
                        <table>
                            <tr><td>1   <td>UET_NVM_TYPE_EEPROM
                            <tr><td>2   <td>UET_NVM_TYPE_SPIFLASH
                        </table>

                    address -
                        The word address in NVM to read from.

                    data -
                        The data read. Must point to an array large enough to hold the 
                        number of words specified by the length parameter.

                    length -
                        The number of words to read.

    Returns :       Status of the command (error code).

    Description :   This function reads a block of data from a specified address in 
                    Non-Volatile Memory.
                    <p>In order to read from non-volatile memory, this function writes 
                    a program to RAM. This overwrites any existing ptest firmware, 
                    which means that the ptest firmware must be reloaded before any 
                    further hardware or RF test operations can be performed. Therefore 
                    it is most efficient to read from NVM before or after hardware 
                    tests.

    Example :

        static const uint32 READ_ADDRESS = 0xF000;

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            // Read some data from NVM
            uint16 data[4];
            if(retVal == UET_OK)
            {
                retVal = uetNvmRead(handle, UET_NVM_TYPE_EEPROM, READ_ADDRESS,
                                    data, sizeof(data) / sizeof(data[0]));
            }

            // Process the data

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetNvmRead(CsrHandle_t handle, uint8 type, uint32 address, 
                                  uint16* data, uint16 length);

/*************************************************************************************

    Function :      int32 uetNvmSetSFlashPios(CsrHandle_t handle, uint8 pioSclk, 
                                              uint8 pioMiso, uint8 reserved);

    Parameters :    handle - 
                        Device connection handle.

                    pioSclk -
                        The PIO that is connected to SPI SCLK.

                    pioMiso -
                        The PIO that is connected to SPI MISO.

                    reserved -
                        Reserved parameter (currently unused).

    Returns :       Status of the command (error code).

    Description :   Sets the PIOs that are connected to the serial flash. If this 
                    function is not used to set the PIOs, default values will be 
                    used (SCLK = 9, MISO = 10).
                    <p>If it is necessary to override the default serial flash PIOs,
                    this function should be called before any operation that reads 
                    from or writes to SPI-Flash, i.e. uetNvmRead, uetNvmWrite, 
                    uetNvmCustomRead, uetNvmCustomWrite, uetCsCacheReadFromChip, 
                    and uetCsCacheWriteToChip. The function only needs to be called 
                    once, as the settings persist for any subsequent operations until 
                    the connection is closed with uetClose.

    Example :

        CsrHandle_t handle = 0;
        int32 retVal = uetOpen("SPITRANS=USB SPIPORT=0", NULL, &handle);
        if(retVal == UET_OK)
        {
            // Override the default PIOs.
            if(retVal == UET_OK)
            {
                retVal = uetNvmSetSFlashPios(handle, 11, 9, 0);
            }

            // Perform NVM / CS operations

            uetClose(handle);
        }

        // Deal with uetOpen error conditions as in uetOpen example

*************************************************************************************/
UENERGYTEST_API(int32) uetNvmSetSFlashPios(CsrHandle_t handle, uint8 pioSclk, 
                                           uint8 pioMiso, uint8 reserved);

/*************************************************************************************

    Function :      int32 uetReset(CsrHandle_t handle)

    Parameters :    handle - 
                        Device connection handle.

    Returns :       Status of the command (error code).

    Description :   This function resets the device.
                    <p>If a valid application image is present in NVM, it will run 
                    following the reset.
                    <p>NOTE: If ptest firmware has been previously loaded (using 
                    uetLoadPtFirmware), resetting the device will remove it. If 
                    additional tests requiring ptest firmware are to be executed 
                    following reset, uetLoadPtfirmware must be called again to 
                    re-load the ptest firmware.

    Example :       See example code for uetCsCacheOtauApp.

*************************************************************************************/
UENERGYTEST_API(int32) uetReset(CsrHandle_t handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UENERGYTEST_H */
