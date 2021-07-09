// ============================================================================
// ttx_monitor: Opensource TaitoTypeX loader
// by Romhack
// ============================================================================
//
//  Versão reescrita do emulador de JVS, com suporte a multiplos
// comandos por stream.

#include "stdafx.h"
//#include <queue>

//#define JVS_TRUE				0x01
//#define JVS_FALSE				0x00
//#define JVS_STREAM_SIZE		1024
//#define JVS_REPORT_CODE		0x01

//Commands to all nodes
//#define JVS_OP_RESET			0xF0 //Command reserts the bus to the power-on state
//#define JVS_OP_ADDRESS		0xF1 //Command issues an address to the node determined by the sense line
//#define JVS_OP_CHECKSUM_ERROR	0x2F //Retransmit data in case of checksum failure

#define JVS_SYNC_CODE			0xE0 //Codigo Sync vet[0]
#define JVS_COMMAND_REV			0x13 //Get command format revision
#define JVS_BOARD_REV			0x30 //Revisao da placa
#define JVS_COMM_REV			0x10 //Get supported communications versions
#define JVS_ADDR_MASTER			0x00
//The 'status' byte
#define JVS_STATUS_OK			0x01 //Success
#define JVS_STATUS_UNKNOWN_CMD	0x02 //Unknown or unsupported command
#define JVS_STATUS_SUM_ERROR	0x03 //Checksum failure
#define JVS_STATUS_ACK_OVERFLOW	0x04 //Overflow
//The 'report' byte
#define JVS_REPORT_OK			0x01 //Success
#define JVS_REPORT_INVAL_PARAM1	0x02 //These have to do with parameter errors
#define JVS_REPORT_INVAL_PARAM2	0x03 //These have to do with parameter errors
#define JVS_REPORT_BUSY			0x04 //Device busy

#define JVS_IOFUNC_SWINPUT		0x01 //Number of players and switches
#define JVS_IOFUNC_COINTYPE		0x02 //Coin slots
#define JVS_IOFUNC_EXITCODE		0x00 //Exit code

#define JVS_SUPPORT_PLAYERS		2
#define JVS_SUPPORT_SLOTS		2

typedef signed char SINT8;
typedef signed short SINT16;
typedef signed short SINT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
//typedef unsigned short UINT32;

static const char *IO_Id = "NBGI.;NA-JV;Ver6.01;JPN,MK3100-1-NA-APR0-A01";

long STEERINGv = 0x7F;
long ACCELv = 0;
long BRAKEv = 0;
DWORD vSTEER[5] = { 15, 20, 35, 50, 65 };
// ============================================================================
//
// ============================================================================
static inline /*unsigned*/ long mapRange(unsigned long x, unsigned long in_min, unsigned long in_max, unsigned long out_min, unsigned long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// ============================================================================
//
// ============================================================================
static inline /*unsigned*/ long mapRange2(unsigned long x, unsigned long in_min, unsigned long in_max, unsigned long out_min, unsigned long out_max) {
	return ((x - in_max) * (out_max - out_min) / (in_max - in_min) + out_max);
}
// ============================================================================
//
// ============================================================================
void/*DWORD*/ __stdcall STEERING() {
	/*
	while (1) {
		memset((void*)0x19D4F8, 0, 0x28);
		*(DWORD*)(0x19D4F8 + 0x34) = 0;
	}
	return;
	*/
	while (1) {
		DWORD ret = STEERINGv; //0x7F
		DWORD value = inputMgr.GetState(P1_RIGHT);
		if (value) {
			ret = STEERINGv += vSTEER[configMgr.GetConfig(CONFIG_NSC_COINCREDIT)];
			if (STEERINGv > 0xFC) ret = STEERINGv = 0xFC;
		} else {
			value = inputMgr.GetState(P1_LEFT);
			if (value) { //<--
				ret = STEERINGv -= vSTEER[configMgr.GetConfig(CONFIG_NSC_COINCREDIT)];
				if (STEERINGv < 0) ret = STEERINGv = 0;
			} else {
				if (STEERINGv > 0x7E) {
					ret = STEERINGv -= vSTEER[configMgr.GetConfig(CONFIG_NSC_COINCREDIT)];
					if (STEERINGv < 0x7E) ret = STEERINGv = 0x7E;
				} else if (STEERINGv < 0x7E) { //<--
					//ret = STEERINGv += vSTEER[configMgr.GetConfig(CONFIG_NSC_COINCREDIT)];
					//if (STEERINGv > 0x7E) ret = STEERINGv = 0x7E;
					if (STEERINGv > 0x8C) ret = STEERINGv -= vSTEER[configMgr.GetConfig(CONFIG_NSC_COINCREDIT)];
					else if (STEERINGv < 0x6C) ret = STEERINGv += vSTEER[configMgr.GetConfig(CONFIG_NSC_COINCREDIT)];
					else ret = STEERINGv = 0x7E;
				}
			}
		}
		Sleep(30); //(configMgr.GetConfig(CONFIG_PTDELAYV));
		//return ret;
	}
}
// ============================================================================
//
// ============================================================================
DWORD __stdcall PEDAL(TTX_InputsDef button, long *var) {
	DWORD ret = 0;
	DWORD value = inputMgr.GetState(button);
	if (value) {
		ret = *var += vSTEER[configMgr.GetConfig(CONFIG_NSC_COINCREDIT)];
		if (*var > 0x54) ret = *var = 0x54;
	} else {
		ret = *var -= vSTEER[configMgr.GetConfig(CONFIG_NSC_COINCREDIT)];
		if (*var < 0x0) ret = *var = 0x0;
	}
	return ret;
}
// ============================================================================
//
// ============================================================================
/*
* Para cada pacote recebido temos:
*
* +-----------+---------------+------+-------+-------+----------+
* | SYNC_CODE | SLAVE_ADDRESS | SIZE | FUNC1 | FUNCn | CHECKSUM |
* +-----------+---------------+------+-------+-------+----------+
*
* Para cada pacote enviado temos:
*
* +-----------+--------------------+------+--------+---------+---------+----------+
* | SYNC_CODE | MASTER_ADDRESS (0) | SIZE | STATUS | REPORT1 | REPORTn | CHECKSUM |
* +-----------+--------------------+------+--------+---------+---------+----------+
* 
* Para cada função recebida é necessário um REPORT, depois do report temos os valores
* da resposta do comando.
*/
/*
struct jvs_command_def {
	UINT8 params;
	UINT8 reports;
};
*/
// ============================================================================
//
// ============================================================================
class InputInfo {
	short contgear = 0;
	DWORD lastStateUpRace = 0;
	DWORD lastStateDownRace = 0;
public:
	InputInfo() { }
	~InputInfo() { }
	void GetPressJoys() {
		inputMgr.ClearState();
		if (joytype < 0x2) inputMgr.Poll_Direct(NULL);
		if (!joytype || joytype == 0x2) inputMgr.PollXBox_Direct(NULL);
		else if (joytype == 0x3) inputMgr.PollWinmm_Direct(NULL, nj);
		else if (joytype == 0x4)  inputMgr.PollKeyboard_Direct(NULL);
		if (inputMgr.GetState(TTX_EXIT_CODE)) ExitProcess(-1); //Para qdo o botao de saida do codigo é ativado
	}
/*	hi bits:
	0 - BUT 2
	1 - BUT 1
	2 - LEFT
	3 - RIGHT
	4 - DOWN
	5 - UP
	6 - SERVICE
	7 - 
	8 - START

	lo bits:
	0 -
	1 -
	2 -
	3 -
	4 - BUT 6
	5 - BUT 5
	6 - BUT 4
	7 - BUT 3	*/
	BYTE Xp1HiByte() {
		BYTE s = 0;
		if (inputMgr.GetState(P1_START)) s |= 0x80;
		if (inputMgr.GetState(P1_SERVICE)) s |= 0x40;

		if (inputMgr.GetState(P1_UP)) s |= 0x20;
		if (inputMgr.GetState(P1_DOWN)) s |= 0x10;
		if (inputMgr.GetState(P1_RIGHT)) s |= 0x04;
		if (inputMgr.GetState(P1_LEFT)) s |= 0x08;

		if (inputMgr.GetState(P1_BUTTON_1))	s |= 0x20; //Move Up
		if (inputMgr.GetState(P1_BUTTON_3))	s |= 0x02; //Enter Switch
		if (inputMgr.GetState(P1_BUTTON_2))	s |= 0x10; //Move Down
		return s;
	}
	BYTE Xp1LoByte() {
		BYTE s = 0;
		if (inputMgr.GetState(P1_BUTTON_4)) s |= 0x02; //Botão Ponto de Vista
		if (inputMgr.GetState(P1_BUTTON_5))	s |= 0x01; //Botão de Insurgência
		
		DWORD justPressUpRace = inputMgr.GetState(P1_BUTTON_8); //Gear Up
		if (justPressUpRace > 1) justPressUpRace = 1;
		justPressUpRace &= ~lastStateUpRace;
		if (justPressUpRace & 1) contgear++;

		DWORD justPressDownRace = inputMgr.GetState(P1_BUTTON_9); //Gear Down
		if (justPressDownRace > 1) justPressDownRace = 1;
		justPressDownRace &= ~lastStateDownRace;
		if (justPressDownRace & 1) contgear--;
		
		if (contgear > 6) contgear = 6;
		else if (contgear < 0) contgear = 0;

		if (inputMgr.GetState(P2_BUTTON_1)) contgear = 1;
		else if (inputMgr.GetState(P2_BUTTON_2)) contgear = 2;
		else if (inputMgr.GetState(P2_BUTTON_3)) contgear = 3;
		else if (inputMgr.GetState(P2_BUTTON_4)) contgear = 4;
		else if (inputMgr.GetState(P2_BUTTON_5)) contgear = 5;
		else if (inputMgr.GetState(P2_BUTTON_6)) contgear = 6;

		switch (contgear) {
			case 1: s |= 0xA0; //1
				break;
			case 2: s |= 0x60; //2
				break;
			case 3: s |= 0x80; //3
				break;
			case 4: s |= 0x40; //4
				break;
			case 5: s |= 0x90; //5
				break;
			case 6: s |= 0x50; //6
				break;
			default: break;
		}
		
		lastStateUpRace = inputMgr.GetState(P1_BUTTON_8);
		if (lastStateUpRace > 1) lastStateUpRace = 1;

		lastStateDownRace = inputMgr.GetState(P1_BUTTON_9);
		if (lastStateDownRace > 1) lastStateDownRace = 1;

		return s;
	}
	/*
	BYTE Xp2HiByte() {
		BYTE s = 0;
		if (inputMgr.GetState(P2_START)) s |= 0x80;
		if (inputMgr.GetState(P2_SERVICE)) s |= 0x40;

		int value = inputMgr.GetState(P2_UP);
		if (joytype == 0x3) {
			if (!value || (value != 0x7FFF && value != 0x7EFF)) value = 1;
			else if (value == 0x7FFF || value == 0x7EFF) value = 0;
		}
		if (value) s |= 0x20;
		value = inputMgr.GetState(P2_DOWN);
		if (joytype == 0x3) {
			if (!value || (value != 0x7FFF && value != 0x7EFF)) value = 1;
			else if (value == 0x7FFF || value == 0x7EFF) value = 0;
		}
		if (value) s |= 0x10;
		value = inputMgr.GetState(P2_RIGHT);
		if (joytype == 0x3) {
			if (!value || (value != 0x7FFF && value != 0x7EFF)) value = 1;
			else if (value == 0x7FFF || value == 0x7EFF) value = 0;
		}
		if (value) s |= 0x04;
		value = inputMgr.GetState(P2_LEFT);
		if (joytype == 0x3) {
			if (!value || (value != 0x7FFF && value != 0x7EFF)) value = 1;
			else if (value == 0x7FFF || value == 0x7EFF) value = 0;
		}
		if (value) s |= 0x08;
		if (inputMgr.GetState(P2_BUTTON_1)) s |= 0x02;
		if (inputMgr.GetState(P2_BUTTON_2)) s |= 0x01;
		return s;
	}
	BYTE Xp2LoByte() {
		BYTE s = 0;
		if (inputMgr.GetState(P2_BUTTON_3))	s |= 0x80;
		if (inputMgr.GetState(P2_BUTTON_4))	s |= 0x40;
		if (inputMgr.GetState(P2_BUTTON_5))	s |= 0x20;
		if (inputMgr.GetState(P2_BUTTON_6))	s |= 0x10;
		return s;
	}
	*/
};
// ============================================================================
//
// ============================================================================
InputInfo inpInfo;
#define __ARG__(n) ((DWORD)pfunc[n])
#define PUSH_ZERO r.push(0x80); r.push(0)
// ============================================================================
//
// ============================================================================
class jprot_encoder {
	BYTE buffer[1024];
	DWORD statusaddr;
	DWORD sizeaddr;
	DWORD sumaddr;
	DWORD xpos;
	DWORD size_;
public:
	DWORD size() { return size_; }
	jprot_encoder() {
		for (DWORD i = 0; i != 1024; i++) buffer[i] = 0; //memset(buffer, 0, 1024);
		xpos = 0;
	}
	~jprot_encoder() {
		for (DWORD i = 0; i != 1024; i++) buffer[i] = 0; //memset(buffer, 0, 1024);
		xpos = 0;
	}
	void set_status(BYTE v) { buffer[statusaddr] = v; }
	void clear() {
		for (DWORD i = 0; i != 1024; i++) buffer[i] = 0; //memset(buffer, 0, 1024);
		size_ = 0;
		xpos = 0;
	}
	void begin_stream() {
		pushz(JVS_SYNC_CODE);
		sumaddr = xpos;
		pushz(JVS_ADDR_MASTER);
		sizeaddr = xpos;
		push(0);
		statusaddr = xpos;
		push(JVS_STATUS_OK); // STATUS
	}
	void pushz(BYTE v) { buffer[xpos++] = v; }
	void push(BYTE v) {
/*		buffer[xpos]=v;
		++xpos; */
		if(v == 0xD0) { //Para 0xD0 a area de escape é 0xD0 0xCF
			buffer[xpos++] = 0xD0;
			buffer[xpos++] = 0xCF;
		} else if(v == 0xE0) { //Para 0xE0 a area de escape é 0xD0 0xDF
			buffer[xpos++] = 0xD0;
			buffer[xpos++] = 0xDF;
		} else buffer[xpos++] = v;
	}
	void report(BYTE v) { push(v); }
	void end_stream() {
/*
		if (xpos == (statusaddr+1)) {
			clear();
			return;
		}
		// codifica o tamanho da stream
		buffer[sizeaddr] = (BYTE) xpos-sizeaddr;

		// calcula o checksum
		DWORD sum = 0;
		for (DWORD i=sumaddr ; i<xpos; i++) {
			sum += (DWORD) (buffer[i] & 0xFF);
		}
		sum &= 0xFF;
		if ((sum == 0xD0) || (sum == 0xE0)) {
			push(0xD0);
			push(sum-1);
		} else
			push(sum);
		size_ = xpos;
*/
		if (xpos == (statusaddr+1)) {
			clear();
			return;
		}
		DWORD sizeK = 0;
		for (DWORD i = sizeaddr; i < xpos; i++) if (buffer[i] != 0xD0) ++sizeK;
// codifica o tamanho da stream
		buffer[sizeaddr] = (BYTE)sizeK; //buffer[sizeaddr] = sizeK;
// calcula o checksum
		DWORD sum = 0;
		for (DWORD i = sumaddr, inc = 0 ; i < xpos; i++) {
			if (buffer[i] == 0xD0) inc = 1;
			else {
				sum += (DWORD) ((buffer[i] + inc) & 0xFF);
				if (inc) inc = 0;
			}
		}
		sum &= 0xFF;
		push((BYTE)sum); //push(sum);
		size_ = xpos;
	}
	void read(BYTE *dst, DWORD size) {
		if (size > size_) size = size_;
		//memcpy(dst, buffer, size);
		BYTE *ptr = (BYTE*)buffer;
		for (DWORD i = 0; i != size; i++) *dst++ = *ptr++;
	}
/*	void print() {
		if (size()) {
			logmsg("WR:  ");
			for(DWORD i=0;i<size();i++)	logmsg("%02X ", buffer[i]);
			logmsg("\n");
		}
	}*/
};
static int isAddressed = 0;
int is_addressed() { return isAddressed; }
void reset_addressed() { isAddressed = 0; }
static WORD p1coin = 0;
static WORD p2coin = 0;
static int coinstate[2]= { 0, 0 };
BYTE test_flag = 0;
// ============================================================================
//
// ============================================================================
DWORD process_stream(UINT8 *stream, BYTE *dst, DWORD dstsize) {
	DWORD lastStateTestMenu = 0;
	jprot_encoder r;
	BYTE *pstr = stream;
	//BYTE node = pstr[1];
	BYTE *pfunc = &pstr[3];
	BYTE pktsize = pstr[2];
	int pktcount = (int)pktsize - 1;
	//if (pstr[0] != JVS_SYNC_CODE) logmsg("SYNC_CODE inválido!\n");
/* pktcount é o número de bytes disponíveis para a stream de funções */
	r.clear();
	r.begin_stream();
	while (pktcount > 0) {
		int increment = 0;
		switch (pfunc[0] & 0xFF) {
/////////////////////////////////////////////////////// (Broadcast commands(meant for all active nodes, sent to node ID FF)) ///////////////////////////////////////////////////////
		case 0xF0: { //Reset
			p1coin = 0;
			p2coin = 0;
			increment = 2;
			break;
		}
		case 0xF1: { //Set node address
			r.report(JVS_REPORT_OK);
			isAddressed = 1;
			//isAddressed++;
			increment = 2;
			break;
		}
		case 0x2F: { //Set up Communications
			increment = 1;
			break;
		}
/////////////////////////////////////////////////////// (Initialization Commands) ///////////////////////////////////////////////////////
		case 0x10: { //Read I/O ID data
				r.report(JVS_REPORT_OK);
				const char *str = IO_Id;
				while (*str) { r.push(*str++); }
				r.push(0);
				increment = 1;
				break;
			}
		case 0x11: { //Get command format revision
			r.report(JVS_REPORT_OK);
			r.push(0x31); //r.push(JVS_COMMAND_REV);
			increment = 1;
			break;
		}
		case 0x12: {// Revisão da placa (JVS)
			r.report(JVS_REPORT_OK);
			r.push(0x31); //r.push(JVS_BOARD_REV);
			increment = 1;
			break;
		}
		case 0x13: { //Get supported communications versions
			r.report(JVS_REPORT_OK);
			r.push(0x31); //r.push(JVS_COMM_REV);
			increment = 1;
			break;
		}
		case 0x14: {// IO REPORT Aqui é onde reportamos as funções suportadas pela JVS
					// através de um subset (JVS_IOFUNC_*) de funções com 3 parâmetros cada uma delas.
			r.report(JVS_REPORT_OK); 
			r.push(JVS_IOFUNC_SWINPUT);
			{
				r.push(JVS_SUPPORT_PLAYERS); // 2 players
				r.push(0x18); // 14 botões
				r.push(0); // null
			}
			r.push(JVS_IOFUNC_COINTYPE);
			{
				r.push(JVS_SUPPORT_SLOTS); // 2 slots
				r.push(0); // null
				r.push(0); // null
			}
			// analog inputs
			r.push(3);
			{
				r.push(8); // channel 1
				r.push(0xA); // 16 bits
				r.push(0); // null
			}
			r.push(0x12);
			{
				r.push(0x14);
				r.push(0);
				r.push(0);
			}
			r.push(0);
			increment = 1;
			break;
		}
		case 0x01: { // Informações sobre o monitor
		    r.report(JVS_REPORT_OK);
		    r.push((BYTE)configMgr.GetConfig(TTX_CONFIG_HIRES));
		    r.push(0);
		    increment = 2;
		   break;
		}
/////////////////////////////////////////////////////// (I / O commands) ///////////////////////////////////////////////////////
		case 0x20: { // Status dos controles digitais //Read Switch inputs
			DWORD justPressTestMenu = inputMgr.GetState(TEST_MODE) ? 1 : 0;
			justPressTestMenu &= ~lastStateTestMenu;

			//logcmd("Lendo controles %X,%X...\n", __ARG__(1), __ARG__(2));
			r.report(JVS_REPORT_OK);
			// test mode, tilt, etc..

			if (justPressTestMenu)  {
				if (!test_flag) test_flag = 1;
				else test_flag = 0;
			}
			if (test_flag) r.push(0x80);
			else r.push(0);

			if (!configMgr.GetConfig(CONFIG_PRESSTYPE)) inpInfo.GetPressJoys();
			r.push(inpInfo.Xp1HiByte());
			r.push(inpInfo.Xp1LoByte());
			r.push(0);
			//r.push(inpInfo.Xp2HiByte());
			//r.push(inpInfo.Xp2LoByte());
			increment = 3;

			lastStateTestMenu = inputMgr.GetState(TEST_MODE) ? 1 : 0;

			break;
		}
		case 0x21: { //Read Coin Inputs
			int currstate = inputMgr.GetState(P1_COIN);
			if (!coinstate[0] && (currstate)) ++p1coin;
			coinstate[0] = currstate;

			//currstate = inputMgr.GetState(P2_COIN);
			//if (!coinstate[1] && (currstate)) ++p2coin;
			//coinstate[1] = currstate;

			//logcmd("Lendo o ficheiro... \n");
			r.report(JVS_REPORT_OK);
			r.push(p1coin >> 8);
			r.push(p1coin & 0xFF);
			//r.push(p2coin >> 8);
			//r.push(p2coin & 0xFF);
			increment = 2;
			//getjoy = 1; //inputMgr.ClearState();
			break;
		}
		case 0x30: { //Decrease number of coins
			WORD val = ((__ARG__(2) & 0xFF) << 8) | (__ARG__(3) & 0xFF);
			r.report(JVS_REPORT_OK);
			switch (__ARG__(1))	{
			case 1:
				if (val > p1coin) p1coin = 0;
				else p1coin -= val; break;
			case 2:
				if (val > p2coin) p2coin = 0;
				else p2coin -= val; break;
			}
			increment = 4;
			break;
		}
		case 0x31: { //output the number of payouts?
			WORD val = ((__ARG__(2) & 0xFF) << 8) | (__ARG__(3) & 0xFF);
			r.report(JVS_REPORT_OK);
			switch (__ARG__(1)) {
				case 1: p1coin += val; break;
				case 2: p2coin += val; break;
			}
			increment = 4;
			break;
		}
// ===========================
		case 0x22: {// read analog...
				r.push(JVS_REPORT_OK);
				int channels = __ARG__(1);
				//wr_analogs:
				//logmsg("cmd 0x22 read analog %x\n", channels);
				
				long counter = 0x7E;
				if (!configMgr.GetConfig(CONFIG_NSC_HOOKSETTINGS)) {
					//dinput
					if (joytype < 0x2) {
						//if (inputMgr.GetState(P1_LEFT)) counter = mapRange(inputMgr.GetState(P1_LEFT), -1000, 1000, 0, 0xFC);
						//else if (inputMgr.GetState(P1_RIGHT)) counter = mapRange(inputMgr.GetState(P1_RIGHT), -1000, 1000, 0, 0xFC);
						if (inputMgr.GetState(P1_LEFT) & 0xF0000000) counter = mapRange(inputMgr.GetState(P1_LEFT), -1000, 1000, 0, 0xFC);
						else if (inputMgr.GetState(P1_LEFT)) counter = mapRange2(inputMgr.GetState(P1_LEFT), -1000, 1000, 0xFC, 0);
						if (inputMgr.GetState(P1_RIGHT) & 0xF0000000) counter = mapRange2(inputMgr.GetState(P1_RIGHT), -1000, 1000, 0xFC, 0);
						else if (inputMgr.GetState(P1_RIGHT)) counter = mapRange(inputMgr.GetState(P1_RIGHT), -1000, 1000, 0, 0xFC);
					}
					//xinput
					else if (/*!joytype || */joytype == 0x2) {
						//if (inputMgr.GetState(P1_LEFT)) counter = mapRange(inputMgr.GetState(P1_LEFT), 0xFFFF8001, 0x7FFF, 0, 0xFC);
						//else if (inputMgr.GetState(P1_RIGHT)) counter = mapRange(inputMgr.GetState(P1_RIGHT), 0xFFFF8001, 0x7FFF, 0, 0xFC);
						if (inputMgr.GetState(P1_LEFT) & 0xF0000000) counter = mapRange(inputMgr.GetState(P1_LEFT), 0xFFFF8001, 0x7FFF, 0, 0xFC);
						else if (inputMgr.GetState(P1_LEFT)) counter = mapRange2(inputMgr.GetState(P1_LEFT), 0xFFFF8001, 0x7FFF, 0xFC, 0);
						if (inputMgr.GetState(P1_RIGHT) & 0xF0000000) counter = mapRange2(inputMgr.GetState(P1_RIGHT), 0xFFFF8001, 0x7FFF, 0xFC, 0);
						else if (inputMgr.GetState(P1_RIGHT)) counter = mapRange(inputMgr.GetState(P1_RIGHT), 0xFFFF8001, 0x7FFF, 0, 0xFC);
					}
				} else counter = STEERINGv;

				// Channel 1 - Steering
				r.push(LOBYTE(counter));
				r.push(HIBYTE(counter));

				counter = 0;
				if (!configMgr.GetConfig(CONFIG_NSC_SCREENVERTICAL)) {
					if (joytype < 0x2) { //dinput
						if (inputMgr.GetState(P1_BUTTON_7) & 0xF0000000) counter = mapRange2(inputMgr.GetState(P1_BUTTON_7), -1000, 0, 0x54, 0);
						else if (inputMgr.GetState(P1_BUTTON_7)) counter = mapRange(inputMgr.GetState(P1_BUTTON_7), 0, 1000, 0, 0x54);
					} else if (/*!joytype || */joytype == 0x2) { //xinput
						if (inputMgr.GetState(P1_BUTTON_7) & 0xF0000000) counter = mapRange2(inputMgr.GetState(P1_BUTTON_7), 0xFFFF8001, 0, 0x54, 0);
						else if (inputMgr.GetState(P1_BUTTON_7)) counter = mapRange(inputMgr.GetState(P1_BUTTON_7), 0, 0x7FFF, 0, 0x54);
					}
				} else counter = PEDAL(P1_BUTTON_7, &ACCELv);

				//Channel 2 - Accel
				r.push(LOBYTE(counter));
				r.push(HIBYTE(counter));

				counter = 0;
				if (!configMgr.GetConfig(CONFIG_NSC_SCREENVERTICAL)) {
					if (joytype < 0x2) { //dinput
						if (inputMgr.GetState(P1_BUTTON_6) & 0xF0000000) counter = mapRange2(inputMgr.GetState(P1_BUTTON_6), -1000, 0, 0x54, 0);
						else if (inputMgr.GetState(P1_BUTTON_6)) counter = mapRange(inputMgr.GetState(P1_BUTTON_6), 0, 1000, 0, 0x54);
					} else if (/*!joytype || */joytype == 0x2) { //xinput
						if (inputMgr.GetState(P1_BUTTON_6) & 0xF0000000) counter = mapRange2(inputMgr.GetState(P1_BUTTON_6), 0xFFFF8001, 0, 0x54, 0);
						else if (inputMgr.GetState(P1_BUTTON_6)) counter = mapRange(inputMgr.GetState(P1_BUTTON_6), 0, 0x7FFF, 0, 0x54);
					}
				} else counter = PEDAL(P1_BUTTON_6, &BRAKEv);

				// Channel 3 - Brake
				r.push(LOBYTE(counter));
				r.push(HIBYTE(counter));

				// Channel 4 - ???
				r.push(0);
				r.push(0);

				// Channel 5 - ???
				r.push(0);
				r.push(0);

				// Channel 6 - ???
				r.push(0);
				r.push(0);

			    increment = 2;
			    break;
		}
		case 0x32: {
			int tags = __ARG__(1);
			r.push(JVS_REPORT_OK);
			increment = 2 + tags;
			break;
		}
		case 0x78: {
			int rsps = __ARG__(1);
			r.push(JVS_REPORT_OK);
			increment = 0xF;// rsps;
			break;
		}
		case 0x79: {
			int rsps = __ARG__(1);
			r.push(JVS_REPORT_OK);
			increment = 0xF;// rsps;
			break;
		}
		case 0x7A: {
			int rsps = __ARG__(1);
			r.push(JVS_REPORT_OK);
			increment = 0xF;// rsps;
			break;
		}
		case 0x70: {
			r.push(JVS_REPORT_OK);
			increment = 2;
			break;
		}
		case 0x7F: {
			r.push(JVS_REPORT_OK);
			increment = 2;
			break;
		}
		case 0x33: { // send something...
			int rsps = __ARG__(1) * 2;
			r.push(JVS_REPORT_OK);
			increment = 2 + rsps;
			break;
		}
		case 0x23:{
			r.push(JVS_REPORT_OK);
			increment = 2;
			break;
		}
		case 0x34: { // LED message
		    int chars = __ARG__(1);
		    r.push(JVS_REPORT_OK);
			increment = 2 + chars;
			break;
		}
		case 0x65: { // ????
			r.push(JVS_REPORT_OK);
			r.push(0);
			r.push(0);
			increment = 2;
			break;
		}
		case 0x6F:
			r.push(JVS_REPORT_OK);
			increment = 2;
			break;
		case 0x6D:
			r.push(JVS_REPORT_OK);
			increment = 2;
			break;
		case 0x6B:
			r.push(JVS_REPORT_OK);
			increment = 2;
			break;
		case 0x6A:
			r.push(JVS_REPORT_OK);
			increment = 9;
			break;
//====================================
		default:
			r.report(JVS_REPORT_OK);
			increment = 1;
			break;
		}
		pktcount -= increment;
		pfunc += increment;
	}
	r.set_status(JVS_STATUS_OK);
	r.end_stream();
	r.read(dst, dstsize);
	//r.print();
	return r.size();
}
// ============================================================================
//
// ============================================================================

DWORD process_streamCOM2(UINT8 *stream, BYTE *dst, DWORD dstsize) {
	DWORD pos = 0;
	/*
	DWORD pktcount = dstsize;
	BYTE *pfunc = stream;
	while (pktcount > 0) {
		int increment = 0;
		switch (pfunc[0]) {
			case 0x30: {
				dst[pos] = 1;
				increment = 1;
				pos++;
			}
			break;
			case 0x21: {
				dst[pos] = 1;
				increment = 1;
				pos++;
			}
			break;
			case 0xD: {
				dst[pos] = 1;
				increment = 1;
				pos++;
			}
			break;
			case 0x76: {
				dst[pos] = 1;
				increment = 1;
				pos++;
			}
			break;
			case 0x50: {
				dst[pos] = 1;
				increment = 1;
				pos++;
			}
			break;
		}
		pktcount -= increment;
		pfunc += increment;
	}
	*/
	//char* str = "NBI.;WinArc;Ver2.04;JPN";
	//BYTE *ptr = (BYTE*)dst;
	//BYTE *ptr2 = (BYTE*)str;
	//for (BYTE i = 0; i != lstrlenA(str); i++) *ptr++ = *ptr2++;
	
	dst[pos++] = 0;
	dst[pos++] = 0;
	dst[pos++] = 0xFF;
	dst[pos++] = 0x3;
	dst[pos++] = 0xFD;
	dst[pos++] = 0xD4;
	dst[pos++] = 0x10;
	dst[pos++] = 0x2;
	dst[pos++] = 0x1A;
	dst[pos++] = 0;
	/*
	dst[pos++] = 0x55;

	dst[pos++] = 0;
	dst[pos++] = 0;
	dst[pos++] = 0xFF;
	dst[pos++] = 0;
	dst[pos++] = 0xFF;
	dst[pos++] = 0;

	dst[pos++] = 0;
	dst[pos++] = 0;
	dst[pos++] = 0xFF;
	dst[pos++] = 0x3;
	dst[pos++] = 0xFD;
	dst[pos++] = 0xD4;
	dst[pos++] = 0x18;
	dst[pos++] = 0x1;
	dst[pos++] = 0x13;
	dst[pos++] = 0;
	*/
	return pos;
}