#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <os_type.h>
//#include "vasFgProject.h"
#include <siso_genicam.h>
#include <sisoIo.h>
#include <fgrab_prototyp.h>
#include <fgrab_struct.h>
#include <fgrab_define.h>

int main ()
{
	// ---------------------------------------------------------------------------
	// Frame Grabber: Initialization
	// ---------------------------------------------------------------------------

	int nrOfCycles = 5000; // Acquire 5000 frames, TODO: insert the number according to your needs
	unsigned int boardIndex = 0; // TODO: Use the correct board index at multi frame grabber systems
	int rc = 0; // return code

	Fg_Struct* fg = Fg_Init("Acq_SingleCXP6x4AreaGray8.dll", boardIndex);
	if(fg == NULL){
		fprintf(stderr, "Error in Fg_Init(): %s\n", Fg_getLastErrorDescription(NULL));
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
		return -1;
	}
	
	int nrOfParameter = Fg_getNrOfParameter(fg);
	fprintf(stderr, "No. of Parameter: %d\n", nrOfParameter);

	int i;
	/*
	for (i = 0; i < nrOfParameter; i++) {
		const char *name = Fg_getParameterName(fg,i);
		fprintf(stderr," Param %d: %s,%x\n",i,name,Fg_getParameterId(fg,i));
	}
	*/

	// ---------------------------------------------------------------------------
	// Frame Grabber: Allocate memory for buffer(s)
	// ---------------------------------------------------------------------------

	// Feature Cnt 0,0 
	int width0 = 5120;
	int height0 = 5120;
	int byteWidth0 = 1;
	size_t bufSize0 = width0 * height0 * (size_t)byteWidth0;
	const unsigned int mem0_bufs = 4;
	dma_mem *pmem0 = Fg_AllocMemEx(fg, bufSize0 * mem0_bufs, mem0_bufs);
	if(pmem0 == NULL){
		fprintf(stderr, "Error in Fg_AllocMemEx(): %s\n", Fg_getLastErrorDescription(fg));
		Fg_FreeGrabber(fg);
		fg = NULL;
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
		return -1;
	}

	rc = Fg_setParameter(fg,FG_WIDTH,&width0,0);
	rc = Fg_setParameter(fg,FG_HEIGHT,&height0,0);

	int atmMode = ATM_SOFTWARE;
	rc = Fg_setParameter(fg,FG_AREATRIGGERMODE,&atmMode,0);

	int trgState = TS_ACTIVE; //TS_SYNC_STOP;  // TS_ACTIVE;
	rc = Fg_setParameter(fg,FG_TRIGGERSTATE,&trgState,0);
	//fprintf(stderr, "Error in Fg_setParameter(): %s\n", Fg_getLastErrorDescription(fg));

	

	// ---------------------------------------------------------------------------
	// Frame Grabber: Create the display(s)
	// ---------------------------------------------------------------------------

	int id0 = CreateDisplay(8 * byteWidth0, width0, height0);
	if(id0 < 0) {
		fprintf(stderr, "Error in CreateDisplay(): %s (%d)\n", Fg_getLastErrorDescription(fg), id0);
		Fg_FreeMemEx(fg, pmem0);
		pmem0 = NULL;
		Fg_FreeGrabber(fg);
		fg = NULL;
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
		return id0;
	}
	SetBufferWidth(id0, width0, height0);


	// ---------------------------------------------------------------------------
	// CoaXPress: Initialization
	// ---------------------------------------------------------------------------

	SgcBoardHandle * bh = NULL;
	rc = Sgc_initBoard(fg, 0, &bh);
	if(rc < 0) {
		fprintf(stderr, "Error in Sgc_initBoard(): %s (%d)\n", Sgc_getErrorDescription(rc), rc);
		CloseDisplay(id0);
		Fg_FreeMemEx(fg, pmem0);
		pmem0 = NULL;
		Fg_FreeGrabber(fg);
		fg = NULL;
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
		return rc;
	}


	// ---------------------------------------------------------------------------
	// CoaXPress: Scan ports for available camera(s)
	// ---------------------------------------------------------------------------

	unsigned int DiscoveryTimeout = 5000; // 5 sec.
	unsigned int LinkSpeed = 0;		// CXP_SPEED_3125;
	unsigned int Port = 0x0F;
	rc = Sgc_scanPorts(bh, Port, DiscoveryTimeout, LinkSpeed);
	if(rc < 0){
		fprintf(stderr, "Error in Sgc_scanPorts(): %s (%d)\n", Sgc_getErrorDescription(rc), rc);
		Sgc_freeBoard(bh);
		bh = NULL;
		CloseDisplay(id0);
		Fg_FreeMemEx(fg, pmem0);
		pmem0 = NULL;
		Fg_FreeGrabber(fg);
		fg = NULL;
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
		return rc;
	}


	// ---------------------------------------------------------------------------
	// CoaXPress: Get the camera A
	// ---------------------------------------------------------------------------

	SgcCameraHandle * chPORT_A = NULL;
	unsigned int portIndexA = PORT_A;
	rc = Sgc_getCamera(bh, portIndexA, &chPORT_A);
	if(rc < 0){
		fprintf(stderr, "Port A: Error in Sgc_getCamera(): %s (%d)\n", Sgc_getErrorDescription(rc), rc);
		Sgc_freeBoard(bh);
		bh = NULL;
		CloseDisplay(id0);
		Fg_FreeMemEx(fg, pmem0);
		pmem0 = NULL;
		Fg_FreeGrabber(fg);
		fg = NULL;
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
		return rc;
	}


	// ---------------------------------------------------------------------------
	// CoaxPress: Connect to the camera A
	// ---------------------------------------------------------------------------

	rc = Sgc_connectCamera(chPORT_A);
	if(rc < 0){
		fprintf(stderr, "Port A: Error in Sgc_connectCamera(): %s (%d)\n", Sgc_getErrorDescription(rc), rc);
		Sgc_freeBoard(bh);
		bh = NULL;
		CloseDisplay(id0);
		Fg_FreeMemEx(fg, pmem0);
		pmem0 = NULL;
		Fg_FreeGrabber(fg);
		fg = NULL;
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
		return rc;
	}


	// ---------------------------------------------------------------------------
	// CoaxPress: Set the size of the output image from camera A
	// ---------------------------------------------------------------------------

	rc = Sgc_setIntegerValue(chPORT_A, "Width", width0);
	if(rc < 0){
		fprintf(stderr, "Port A: Error in Sgc_setIntegerValue(): %s (%d)", Sgc_getErrorDescription(rc), rc);
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
	}

	rc = Sgc_setIntegerValue(chPORT_A, "Height", height0);
	if(rc < 0) {
		fprintf(stderr, "Port A: Error in Sgc_setIntegerValue(): %s (%d)", Sgc_getErrorDescription(rc), rc);
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
	}

	// /*
	rc = Sgc_setEnumerationValue(chPORT_A, "TriggerMode", "On");
	if(rc < 0) {
		fprintf(stderr, "Port A: Error in Sgc_setEnumerationValue(): %s (%d)", Sgc_getErrorDescription(rc), rc);
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
	}
	// */


	// ---------------------------------------------------------------------------
	// Frame Grabber: Start acquisition at applet and camera for each present port
	// ---------------------------------------------------------------------------

	// 	/*
	rc = Fg_AcquireEx(fg, 0, nrOfCycles, ACQ_STANDARD, pmem0);
	if(rc != 0) {
		fprintf(stderr, "Error in Fg_AcquireEx() for channel 0: %s\n", Fg_getErrorDescription(fg, rc));
		Sgc_disconnectCamera(chPORT_A);
		Sgc_freeBoard(bh);
		bh = NULL;
		CloseDisplay(id0);
		Fg_FreeMemEx(fg, pmem0);
		pmem0 = NULL;
		Fg_FreeGrabber(fg);
		fg = NULL;
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
		return rc;
	}
	// 	*/

	// ---------------------------------------------------------------------------
	// CoaXPress: Start acquisition from camera A
	// ---------------------------------------------------------------------------

	rc = Sgc_startAcquisition(chPORT_A, true);
	if(rc < 0) {
		fprintf(stderr, "Port A: Error in Sgc_startAcquisition(): %s (%d)\n", Sgc_getErrorDescription(rc), rc);
		Fg_stopAcquireEx(fg, 0, pmem0, 0);
		Sgc_disconnectCamera(chPORT_A);
		Sgc_freeBoard(bh);
		bh = NULL;
		CloseDisplay(id0);
		Fg_FreeMemEx(fg, pmem0);
		pmem0 = NULL;
		Fg_FreeGrabber(fg);
		fg = NULL;
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
		return rc;
	}

	// ---------------------------------------------------------------------------
	// Frame Grabber: Grab images
	// ---------------------------------------------------------------------------

	frameindex_t frameNo0 = 0;
	for(i = 1; i <= nrOfCycles; ++i){
		frameNo0 = Fg_getLastPicNumberBlockingEx(fg, frameNo0 + 1, 0, 5, pmem0);
		if(frameNo0 < 0) {
			fprintf(stderr, "Error in Fg_getLastPicNumberBlockingEx(): %s (%d)\n", Fg_getLastErrorDescription(fg), frameNo0);
			fprintf(stderr, "Press any key to continue...\n");
			_getch();
			break;
		} else {
			fprintf(stderr, "grabbed image %d\n", frameNo0 );
		}

		void *iPtr0 = Fg_getImagePtrEx(fg, frameNo0, 0, pmem0);
		if(iPtr0 == NULL) {
			fprintf(stderr, "Error in Fg_getImagePtrEx(): %s\n", Fg_getLastErrorDescription(fg));
			Sgc_stopAcquisition(chPORT_A, true);
			Fg_stopAcquireEx(fg, 0, pmem0, 0);
			Sgc_disconnectCamera(chPORT_A);
			Sgc_freeBoard(bh);
			bh = NULL;
			CloseDisplay(id0);
			Fg_FreeMemEx(fg, pmem0);
			pmem0 = NULL;
			Fg_FreeGrabber(fg);
			fg = NULL;
			fprintf(stderr, "Press any key to continue...\n");
			_getch();
			return -1;
		}
		DrawBuffer(id0, iPtr0, frameNo0, 0);

	}


	// ---------------------------------------------------------------------------
	// CoaxPress: Stop acquisition from camera A
	// ---------------------------------------------------------------------------

	rc = Sgc_stopAcquisition(chPORT_A, true);
	if(rc < 0) {
		fprintf(stderr, "Port A: Error in Sgc_stopAcquisition(): %s (%d)\n", Sgc_getErrorDescription(rc), rc); 
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
	}


	// ---------------------------------------------------------------------------
	// Frame Grabber: Stop acquisition
	// ---------------------------------------------------------------------------

	rc = Fg_stopAcquireEx(fg, 0, pmem0, 0);
	if(rc < 0) {
		fprintf(stderr, "Error in Fg_stopAcquireEx(): %s (%d)\n", Fg_getLastErrorDescription(fg), rc);
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
	}


	// ---------------------------------------------------------------------------
	// CoaxPress: Disconnect from the camera A
	// ---------------------------------------------------------------------------

	rc = Sgc_disconnectCamera(chPORT_A);
	if(rc < 0) {
		fprintf(stderr, "Port A: Error in Sgc_disconnectCamera(): %s (%d)\n", Sgc_getErrorDescription(rc), rc);
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
	}


	// ---------------------------------------------------------------------------
	// CoaxPress: Free the board
	// ---------------------------------------------------------------------------

	Sgc_freeBoard(bh);
	bh = NULL;


	// ---------------------------------------------------------------------------
	// Frame Grabber: Close the display
	// ---------------------------------------------------------------------------

	CloseDisplay(id0);


	// ---------------------------------------------------------------------------
	// Frame Grabber: Release the memory for buffer(s)
	// ---------------------------------------------------------------------------

	rc = Fg_FreeMemEx(fg, pmem0);
	if(rc < 0) {
		fprintf(stderr, "Error in Fg_FreeMemEx(): %s (%d)\n", Fg_getLastErrorDescription(fg), rc);
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
	}
	pmem0 = NULL;


	// ---------------------------------------------------------------------------
	// Frame Grabber: Uninitialization
	// ---------------------------------------------------------------------------

	rc = Fg_FreeGrabber(fg);
	if(rc < 0) {
		fprintf(stderr, "Error in Fg_FreeGrabber(): %s\n", Fg_getLastErrorDescription(NULL));
		fprintf(stderr, "Press any key to continue...\n");
		_getch();
	}
	fg = NULL;


	fprintf(stderr, "Press any key to exit...\n");
	_getch();

	return (0);
}
