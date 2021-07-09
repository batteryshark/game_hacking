// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XIO_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XIO_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef XIO_EXPORTS
#define XIO_API __declspec(dllexport)
#else
#define XIO_API __declspec(dllimport)
#endif

extern XIO_API int nxio;

XIO_API int fnxio(void);

/*
Globals
*/


/*
XIO Specific Stuff
*/
XIO_API class XIO {
public:
	unsigned char lampCoinIOData;
	unsigned char vibrationData;
	unsigned char steeringData;
	unsigned char m3;
	unsigned char m4;
	unsigned char pedalData;
	unsigned char lamp1;
	unsigned char lamp2;
	XIO_API XIO();

	XIO_API ~XIO();

	XIO_API int openInterface();

	XIO_API void closeInterface();

	XIO_API void initialize();

	XIO_API void updateInputState();

	XIO_API void initLamp();

	XIO_API void initVibration();

	XIO_API void initSteering();

	XIO_API void initPedal();

	XIO_API void resetPedal();

	XIO_API void selectLamp(int lampState);

	XIO_API void startLamp(int lampState);

	XIO_API void leftLamp(unsigned char lampState);

	XIO_API void rightLamp(unsigned char lampState);

	XIO_API void lamp(unsigned char lampState);

	void keyboardInput();

	XIO_API void update();

	XIO_API void updateCoinCounter();

	XIO_API void updateHandleState();

	XIO_API unsigned char sendHandleBarState();

	XIO_API unsigned char getHandleState(unsigned char value);

	XIO_API unsigned char calculateHandleState(unsigned char value);





	XIO_API void updatePedalState();
	XIO_API void updateLampState();
	XIO_API void setVibration(unsigned char a2, int a3);

	XIO_API void setSteeringBar(unsigned char inputData);

	XIO_API unsigned char getSteeringBar();

	XIO_API unsigned char getVibration();

};




