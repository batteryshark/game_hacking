/*
I don't see any sense in emulation of responses of the server. There is "manual activation" - on it it is possible to recover with a big accuracy that does the server when receiving "request unlock code" and as it generates "unlock code". I.e. based on this information to make keygen. I to you will make the big help: at the heart of SecuROM PA the following chain of algorithms is put: DES -> DES -> RSA.
RSA is applied to your HWID, the second DES, in fact, a binding to game, the first DES encodes all structure of unlock code.
*/
// ------------------------------------------------------------------------------------
// (!) Struct of SecuROM PA unlock code. Project "80_PA" (!)
// Autor: ELF, 2014. Version: 1.0a
// Published: EXELAB.RU
// ------------------------------------------------------------------------------------
typedef struct sc_imei_part
{
byte T80_IMEI[15];
byte T80_IMEI_Len_ASCII;
}imei_part;

typedef struct ELF_80_PA_UNLOCK_CODE
{
byte T80_CRC_of_right_part;
byte T80_Black_list_count;
WORD T80_CRC_of_MD5_Serial_num;
WORD T80_CRC_of_MD5_DES_PRIMARY_key_digest;
WORD T80_LOCK_INT_DATA;
byte T80_LOCK_TYPE_IDENT;

imei_part imei;

}T_80_unlock;
