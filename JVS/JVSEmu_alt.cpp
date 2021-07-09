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

static const char *IO_Id = "NAMCO LTD.;I/O PCB-1000;ver1.0;for domestic only,no analog input";

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
		if (inputMgr.GetState(P1_SERVICE)) s |= 0x40; //SERVICE
		if (inputMgr.GetState(P1_BUTTON_5))	s |= 0x20; //Partida Esquerda
		if (inputMgr.GetState(P1_BUTTON_6))	s |= 0x10; //Começo Certo
		if (inputMgr.GetState(P1_BUTTON_7))	s |= 0x08; //Canhão

		return s;
	}
	BYTE Xp1LoByte() {
		BYTE s = 0;
		if (inputMgr.GetState(P1_BUTTON_1)) s |= 0x04; //ENTER
		if (inputMgr.GetState(P1_BUTTON_2)) s |= 0x08; //SELECT
		return s;
	}
	BYTE Xp2HiByte() {
		BYTE s = 0;
		if (inputMgr.GetState(P1_BUTTON_3)) s |= 0x40; //SW
		return s;
	}
	BYTE Xp2LoByte() {
		BYTE s = 0;
		return s;
	}
};
// ============================================================================
//
// ============================================================================
InputInfo inpInfo;
#define __ARG__(n) ((DWORD)pfunc[n])
//#define PUSH_ZERO r.push(0xFF); r.push(0xC0)
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
			//logmsg("WR:  ");
			for(DWORD i=0;i<size();i++)	//logmsg("%02X ", buffer[i]);
			//logmsg("\n");
		}
	}*/
};
static int isAddressed = 0;
int is_addressed() { return isAddressed; }
void reset_addressed() { isAddressed = 0; }
static WORD p1coin = 0;
static WORD p2coin = 0;
static int coinstate[2]= { 0, 0 };
DWORD lastStateVolume = 0;
// ============================================================================
//
// ============================================================================
DWORD process_stream(UINT8 *stream, BYTE *dst, DWORD dstsize) {
	jprot_encoder r;
	BYTE *pstr = stream;
	//BYTE node = pstr[1];
	BYTE *pfunc = &pstr[3];
	BYTE pktsize = pstr[2];
	int pktcount = (int)pktsize - 1;
	//if (pstr[0] != JVS_SYNC_CODE) //logmsg("SYNC_CODE inválido!\n");
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
			r.push(JVS_COMMAND_REV);
			increment = 1;
			break;
		}
		case 0x12: {// Revisão da placa (JVS)
			r.report(JVS_REPORT_OK);
			r.push(JVS_BOARD_REV);
			increment = 1;
			break;
		}
		case 0x13: { //Get supported communications versions
			r.report(JVS_REPORT_OK);
			r.push(JVS_COMM_REV);
			increment = 1;
			break;
		}
		case 0x14: {// IO REPORT Aqui é onde reportamos as funções suportadas pela JVS
					// através de um subset (JVS_IOFUNC_*) de funções com 3 parâmetros cada uma delas.
			r.report(JVS_REPORT_OK); 
			r.push(JVS_IOFUNC_SWINPUT);
			{
				r.push(3); // 2 players //JVS_SUPPORT_PLAYERS
				r.push(0x16); // 14 botões
				r.push(0); // null
			}

			r.push(JVS_IOFUNC_COINTYPE);
			{
				r.push(3); // 2 slots //JVS_SUPPORT_SLOTS
				r.push(0); // null //16
				r.push(0); // null
			}

			// analog inputs
			r.push(3);
			{
				r.push(0xA); // channel 1 //3
				r.push(0xA); // 16 bits
				r.push(0); // null
			}
			r.push(7);
			{
				r.push(0);
				r.push(0x16);
				r.push(0);
			}
			r.push(0x12);
			{
				r.push(0x18);
				r.push(0);
				r.push(0);
			}

			r.push(0x11);
			{
				r.push(1);
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
			r.report(JVS_REPORT_OK);
			// test mode, tilt, etc..
			if (inputMgr.GetState(TEST_MODE)) r.push(0x80);
			else r.push(0);
			
			if (!configMgr.GetConfig(CONFIG_PRESSTYPE)) inpInfo.GetPressJoys();
			r.push(inpInfo.Xp1HiByte());
			r.push(inpInfo.Xp1LoByte());
			r.push(inpInfo.Xp2HiByte());
			r.push(inpInfo.Xp2LoByte());
			increment = 3;
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
			//case 2:
				//if (val > p2coin) p2coin = 0;
				//else p2coin -= val; break;
			}
			increment = 4;
			break;
		}
		case 0x31: { //output the number of payouts?
			WORD val = ((__ARG__(2) & 0xFF) << 8) | (__ARG__(3) & 0xFF);
			r.report(JVS_REPORT_OK);
			switch (__ARG__(1)) {
				case 1: p1coin += val; break;
				//case 2: p2coin += val; break;
			}
			increment = 4;
			break;
		}
		case 0x25: {
			int tags = __ARG__(1);
			r.push(JVS_REPORT_OK);
			r.push(0);
			r.push(0);
			r.push(0);
			r.push(0);
			increment = 1;
			break;
		}
		case 0x32: {
			int tags = __ARG__(1);
			BYTE tags2 = __ARG__(2);
			BYTE tags3 = __ARG__(3);
			BYTE tags4 = __ARG__(4);
			//BYTE tags5 = __ARG__(5);
			r.push(JVS_REPORT_OK);


			increment = 2 + tags;
			if (tags2 == 0xD0 || tags3 == 0xD0 || tags4 == 0xD0) increment++;
		}
			break;
		case 0x67:
			r.push(JVS_REPORT_OK);
			increment = 2;
			break;
// ===========================
		case 0x22: {// read analog...
				r.push(JVS_REPORT_OK);
				int channels = __ARG__(1);
				// Channel 1 - ???
				PUSH_ZERO;
				// Channel 2 - ???
				PUSH_ZERO;
				// Channel 3 - ???
				PUSH_ZERO;
				// Channel 4 - ???
				PUSH_ZERO;
			    // Channel 5 - ???
			    PUSH_ZERO;
				// Channel 6 - ???
				PUSH_ZERO;
			    // Channel 7 - ???
			    PUSH_ZERO;
			    // Channel 8 - ???
				PUSH_ZERO;
				// Channel 9 - ???
				PUSH_ZERO;
				// Channel 10 - ???
				PUSH_ZERO;
			    increment = 2;
			    break;
		}
		case 0x33: { // send something...
			int rsps = __ARG__(1) * 2;
			r.push(JVS_REPORT_OK);
			increment = 2 + rsps;
			break;
		}
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
DWORD process_streamLR(UINT8 *stream, BYTE *dst, DWORD dstsize/*nNumberOfBytesToWrite*/) {
	DWORD pos = 0;
	//BYTE *stream = (BYTE *)stream;
	while (pos < dstsize) {
		switch (stream[pos]) {
			case 0x81: {
				dst[1] = 0x42; //???
				for (BYTE i = 0; i != 0x4; i++) dst[0x4] += dst[i] & 0xFF;
				pos += 0x4;
				break;
			}
			default: {
				pos++;
				break;
			}
		}
	}
	return pos;
}