
import java.util.Arrays;

/**
 * LibJVS is (almost) an software implementation of a JAMMA Video System slave.
 * @author Dan Colardeau
 */
public class LibJVS 
{    
    public static final boolean HIGH = true;
    public static final boolean LOW = false;
    public static int TYPE_WHITEPCB = 10;
    public static int TYPE_NONE = -1;
    public static int TYPE_DUMPJVS = 9999;
    private static jvsAdapter adapter;
    private short[] jvsIdentString;
    private short[] jvsOutputBytes;
    private short[] jvsRepeatBytes;
    private short[] theBuffer;
    private short[] flags;
    private short[] temp;
    private int jvsAddress;
    private int outputBytesCount;
    private int outputPosition;
    private int setType;
    private int selectedSlot;
    private boolean jvsInitialized;
    private boolean jvsWrite;
    private boolean jvsChecksum;
    private boolean firstCommand71;
    private boolean senseStatus;
    private boolean inputSense;
    private boolean scribbleCheck;
    //private boolean debugCheck = false;
    /** Generic Constructor for use only with testing
     *  Resets all the internal variables to a fresh state and readies the library to read packets.
     */
    public LibJVS()
    {
        jvsInitialized = false;
        jvsWrite = false;
        jvsChecksum = false;
        jvsAddress = -1;
        senseStatus = false;
        inputSense = LOW;
        setType = TYPE_NONE;
    }
    /**
     * Constructor for specific JVS I/O boards
     * <p>
     * Call this constructor to create a more specific kind of JVS I/O board. Pass a Type and the library will set other parameters accordingly
     * @param type an Integer that represents a specific JVS I/O board
     */
    public LibJVS(int type)
    {
        //Universal shit
        jvsInitialized = false;
        jvsWrite = false;
        jvsChecksum = false;
        jvsAddress = -1;
        senseStatus = false;
        inputSense = LOW;
        setType = type;
        //Not really right now
        if (type == TYPE_WHITEPCB)
        {
            //Fix at some point
            // \x01KONAMI CO.,LTD.;White I/O;Ver1.0;White I/O PCB\x00
            firstCommand71 = false;
            jvsIdentString = new short[]{0xE0, 0x00, 0x32, 0x01, 0x01, 0x4B, 0x4F, 0x4E, 0x41, 0x4D, 0x49, 0x20, 0x43, 0x4F, 0x2E, 0x2C, 0x4C, 0x54, 0x44, 0x2E, 0x3B, 0x57, 0x68, 0x69, 0x74, 0x65, 0x20, 0x49, 0x2F, 0x4F, 0x3B, 0x56, 0x65, 0x72, 0x31, 0x2E, 0x30, 0x3B, 0x57, 0x68, 0x69, 0x74, 0x65, 0x20, 0x49, 0x2F, 0x4F, 0x20, 0x50, 0x43, 0x42, 0x00, 0x43};
            theBuffer = new short[500000];
        }
    }
    public LibJVS(int type, jvsAdapter a)
    {
        adapter = a;
        //Universal shit
        jvsInitialized = false;
        jvsWrite = false;
        jvsChecksum = false;
        jvsAddress = -1;
        senseStatus = false;
        inputSense = LOW;
        setType = type;
        //Not really right now
        if (type == TYPE_WHITEPCB)
        {
            //Fix at some point
            // \x01KONAMI CO.,LTD.;White I/O;Ver1.0;White I/O PCB\x00
            firstCommand71 = false;
            jvsIdentString = new short[]{0xE0, 0x00, 0x32, 0x01, 0x01, 0x4B, 0x4F, 0x4E, 0x41, 0x4D, 0x49, 0x20, 0x43, 0x4F, 0x2E, 0x2C, 0x4C, 0x54, 0x44, 0x2E, 0x3B, 0x57, 0x68, 0x69, 0x74, 0x65, 0x20, 0x49, 0x2F, 0x4F, 0x3B, 0x56, 0x65, 0x72, 0x31, 0x2E, 0x30, 0x3B, 0x57, 0x68, 0x69, 0x74, 0x65, 0x20, 0x49, 0x2F, 0x4F, 0x20, 0x50, 0x43, 0x42, 0x00, 0x43};
            theBuffer = new short[500000];
        }
        adapter.addActionListener(new jvsEvent());
    }
    
    /**
     * Sets JVS status for debugging reasons. Only call this if you know what you're doing or if you want to use LibJVS as a pirate on a real JVS Bus.
     * @param address The JVS Address to Set
     */
    public void setJVSState (int address)
    {
        jvsInitialized = true;
        jvsWrite = false;
        jvsChecksum = false;
        jvsAddress = address;
        senseStatus = false;
        inputSense = LOW;
    }
    /**
     * Returns the JVS checksum for a given packet.
     * <p>
     * Use this function to generate the JVS Checksum of a fresh packet or to verify the JVS Checksum of an existing packet.
     * @param bytes a short array of a JVS Packet you wish to find the JVS Checksum of.
     * @return a single short masked to 8 bits representing the JVS Checksum of the input packet.
     */
    public static short jvsChecksum(short[] bytes) 
    {
        int totals = 0;
        //First Byte of this array is not checked on purpose. E0 sync byte is not factored into checksum byte.
        for (int i = 1; i < ((int)bytes[2] + 2); i++) 
        {
            totals = totals + (int)bytes[i];
        }
        return (short)((totals % 256) & 0xFF);
    }
    /**
     * Returns the node's current output sense line status.
     * <p>
     * Since this library does not handle the manipulation of the sense line, it only tracks if the sense line should be high or low. Use this function to find out what the status of sense should be and then set it inside the application using this library
     * @return A single boolean
     *         true if Sense should be high, false if Sense should be low. You can compare with LibJVS.HIGH or LibJVS.LOW static booleans to help keep it straight.
     */
    public boolean getOutputSense()
    {
        return senseStatus;
    }
    /**
     * Returns if the node should write to the JVS Bus at this time.
     * @return a single boolean. True if this JVS node should respond to the JVS Master at this time, false if the JVS node was not requested in the input packet last read by processRequest(short jvsInputBytes);
     * @see processRequest(short[] jvsInputBytes)
     */
    public boolean jvsWrite()
    {
        return jvsWrite;
    }
    /**
     * Returns the JVS Node's current believed input sense status.
     * <p>
     * This library does not actually read the input sense pin, it is left up to the user to figure out how to do that. It is the program's responsibility to tell LibJVS if it's input sense line is High or Low. 
     * @return a single boolean representing the current believed status of the input sense line. True if high, False if low. You can use statically defined LibJVS.HIGH and LibJVS.LOW to keep them straight.
     * @see setInputSense(boolean input)
     */
    public boolean getInputSense()
    {
        return inputSense;
    }
    /**
     * Sets the current status of the input sense line
     * <p>
     * JVS uses the input and output sense lines to address its nodes correctly. This function is used to tell LibJVS if its input Sense is high or low. True is High, False is low. You can use statically defined LibJVS.HIGH or LibJVS.LOW to keep them straight.
     * @param input A boolean representing the current status of the input sense line.
     */
    public void setInputSense(boolean input)
    {
        inputSense = input;
    }
    /**
     * Strips the input packet down to the minimum required bytes. Very useful when testing length setting.
     * @param input a JVS input packet that may have excess bytes or be corrupted at the length
     * @return a JVS packet that has been sheared off a the JVS Length.
     */
    public static short[] stripPacket (short[] input)
    {
        int len = ((int)input[2]+3);
        short[] output = new short[len];
        System.arraycopy (input, 0, output, 0, len);
        return output;
    }
    /** Prints the JVS packet sent as the bytes parameter to System.out.
     * 
     * @param bytes - JVS Packet to print
     * @param tx - Prints "TX: " in front of the packet if True, "RX: " If false. Useful for writing JVS capture tools.
     */
    public static void printJVSPacket (short[] bytes, boolean tx)
    {
        String temp=new String();
        if (tx)
        {
            System.out.print("TX: ");
        }
        else 
        {
            System.out.print("RX: ");
        }
        for (int i = 0; i < ((int)bytes[2]+3); i++)
        {
            temp=temp+Integer.toHexString((int)bytes[i]);
            temp=temp+" ";
        }
        if (bytes[(int)bytes[2]+2] != LibJVS.jvsChecksum(bytes))
        {
            System.out.println(temp+" <- CHECKSUM FAILED");
        }
        else
        {
            System.out.println(temp+" <- Checksum OK");
        }
    }
    private void process()
    {
        setInputSense(adapter.getInputSense());
        //printJVSPacket(adapter.readPacket(),false);
        short[] response = processRequest(adapter.readPacket());
        adapter.setOutputSense(getOutputSense());
        if (jvsWrite())
        {
            //printJVSPacket(response,true);
            adapter.writePacket(response);
        }
        
    }
    /** Returns a JVS packet in the form of a short array that is the response to the JVS packet sent as the first parameter.
     * <p>
     * The array returned by this function should be sent to the JVS Master within 10ms if jvsWrite() returns true.
     * 
     * @param jvsInputBytes A short array of a packet sent by a JVS Master requesting JVS Slaves to do work. Will be ignored by this node if the JVS Address of this node is not the same as the JVS Address specified in the packet.
     * @return A short array of a packet to be sent to the JVS Master, or an array of 256 0x00 if this slave was not addressed. Check return of jvsWrite() to see if the results of this function are relevant.
     */
    public short[] processRequest (short[] jvsInputBytes)
    {
        //I love ths smell of fresh chars in the morning.
        jvsOutputBytes = new short[256];
        //Gonna use this like 6 times. Just do it once and store it
        jvsChecksum = jvsInputBytes[(int)(jvsInputBytes[2])+2] == jvsChecksum(jvsInputBytes);
        //Fail quickly.
        if (jvsInitialized && (int)jvsInputBytes[1] == jvsAddress && !jvsChecksum) 
        {
            System.out.println("JVS Checksum Mismatch!");
            jvsOutputBytes = new short[]{0xe0, 0x00, 0x02, 0x2F, 0x31};
            //jvsOutputBytes[4] = jvsChecksum(jvsOutputBytes);
            jvsWrite = true;
        }   
        //Check for JVS retransmit request
        if (jvsInitialized && (int)jvsInputBytes[1] == jvsAddress && jvsInputBytes[2] == 0x02 && jvsInputBytes[3] == 0x2F && jvsChecksum)
        {
            jvsOutputBytes = jvsRepeatBytes;
        }
        //Ignore packets not addressed to me
        if (jvsInitialized && jvsInputBytes[1] != jvsAddress && jvsChecksum)
        {
            jvsWrite = false;
        }
        //Packet for me. Do something with it.
        if (jvsInitialized && jvsInputBytes[1] == jvsAddress && jvsChecksum) 
        {
            //JVS Identification Packet <- RONG
            if (jvsInputBytes[0] == 0xE0 && jvsInputBytes[2] == 0x02 && jvsInputBytes[3] == 0x10) 
            {
                System.arraycopy(jvsIdentString, 0, jvsOutputBytes, 0, jvsIdentString.length);
            }
            else 
            {
                //do some resets
                outputBytesCount = 2; //Status and Sync
                outputPosition = 4;
                jvsOutputBytes = new short[256];
                //setup a blank response packet
                jvsOutputBytes[0] = 0xE0;
                jvsOutputBytes[1] = 0x00;
                //Byte [2] is the length short, set later
                jvsOutputBytes[3] = 0x01; //01 means normal response, shouldn't be hardcoded probably.
                //Gigantic Loop From Hell.
                for (int i = 3; i < (int)jvsInputBytes[2] + 2; i++) 
                {
                    //remember to increase i if the request recieved is larger than 1 byte
                    //Command Format Version
                    if (jvsInputBytes[i] == 0x11)
                    {
                        i = i + commandFormatVersion();
                        continue;
                    }
                    //JVS Revision
                    if (jvsInputBytes[i] == 0x12)  
                    {
                        i = i + jvsRevision();
                        continue;
                    }
                    //Protocol Version
                    if (jvsInputBytes[i] == 0x13)   
                    {
                        i = i + protocolVersion();
                        continue;
                    }
                    //Capability listing.
                    if (jvsInputBytes[i] == 0x14)  
                    {
                        i = i + capabilityListing();
                        continue;
                    }
                    //Read Buffer To System 573
                    if (jvsInputBytes[i] == 0x70 && jvsInputBytes[i + 1] == 0x00)  
                    {
                        i = i + whiteReadBuffer(jvsInputBytes, i);
                        continue;
                    }
                    //Write Buffer from System 573
                    if (jvsInputBytes[i] == 0x70 && jvsInputBytes[i+1] == 0x01)   
                    {
                        i = i + whiteWriteBuffer(jvsInputBytes, i);
                        continue;
                    }
                    //Set Execution Address
                    //Lie to the 573. We didn't even copy it's code.
                    if (jvsInputBytes[i] == 0x70 && jvsInputBytes[i + 1] == 0x02) 
                    {
                        i = i + whiteSetExecutionAddress(jvsInputBytes, i);
                        continue;
                    }
                    //73 76 Execute
                    //Lie to the 573. We "Ran its program"
                    if (jvsInputBytes[i] == 0x73 && jvsInputBytes[i + 1] == 0x76) 
                    {
                        i = i + whiteExecuteProgram(i);
                        continue;
                    }
                    //Really important for White PCB, Commands 71, 76
                    //71 Get flags
                    if (jvsInputBytes[i] == 0x71)   
                    {
                        i = i + whiteGetFlags(i);
                        continue;
                    }
                    //76 74 Read Card Into Buffer.
                    if (jvsInputBytes[i] == 0x76 && jvsInputBytes[i+1] == 0x74) 
                    {
                        i = i + whiteReadCard(jvsInputBytes, i);
                        continue;
                    }
                    //76 75 Write Card
                    if (jvsInputBytes[i] == 0x76 && jvsInputBytes[i+1] == 0x75) 
                    {
                        i = i + whiteWriteCard(jvsInputBytes, i);
                        continue;
                    }
                }// <- this is the loop (Which means this loop is probably too long. Oh well. #YOLO
                //Set the length
                jvsOutputBytes[2] = (short)(outputBytesCount & 0xFF);
                //Calculate the checksum
                jvsOutputBytes[outputBytesCount + 2] = jvsChecksum(jvsOutputBytes);
            }
            //Set the write flag
            jvsWrite = true;
        } 
        //Broadcast Packets
        if (jvsInputBytes[1] == 0xFF && jvsChecksum) 
        {
            //JVS Reset Sequence
            jvsBroadcast(jvsInputBytes);
        }
        //Keep a backup of the previous packet in case there's a retransmission request.
        jvsRepeatBytes = new short[256];
        System.arraycopy(jvsOutputBytes, 0, jvsRepeatBytes, 0, jvsOutputBytes.length);
        //Send along the output packet.
        jvsOutputBytes = stripPacket(jvsOutputBytes);
        return jvsOutputBytes;
    }
    /**
     * Poorly coded function that hardcodes the Command Format Version.
     * @return 0, should return 0 because Command format version only reads 1 byte off the input packet and the for loop does that.
     */
    private int commandFormatVersion()
    {
        outputBytesCount = outputBytesCount + 2;
        jvsOutputBytes[outputPosition] = 0x01;
        jvsOutputBytes[outputPosition + 1] = 0x11;
        outputPosition = outputPosition + 2;
        return 0;
    }
    /**
     * Poorly coded function that hardcodes the JVS Revision. 
     * @return 0, JVS revision reads only one byte.
     */
    private int jvsRevision()
    {
        outputBytesCount = outputBytesCount + 2;
        jvsOutputBytes[outputPosition] = 0x01;
        jvsOutputBytes[outputPosition + 1] = 0x20;
        outputPosition = outputPosition + 2;
        return 0;
    }
    /**
     * Poorly coded function that hardcodes the protocol version.
     * @return 0, because protocol version request only reads one byte.
     */
    private int protocolVersion()
    {
        outputBytesCount = outputBytesCount + 2;
        jvsOutputBytes[outputPosition] = 0x01;
        jvsOutputBytes[outputPosition + 1] = 0x10;
        outputPosition = outputPosition + 2;
        return 0;
    }
    /**
     * Poorly coded function that hardcodes the capability listing. This should *REALLY* not be hardcoded.
     * @return 0 because Whitepcbs have a blank capability list.
     */
    private int capabilityListing()
    {
        outputBytesCount = outputBytesCount + 2;
        jvsOutputBytes[outputPosition] = 0x01;
        jvsOutputBytes[outputPosition + 1] = 0x00;
        outputPosition = outputPosition + 2;
        return 0;
    }
    /**
     * Reads buffer from WhitePCB internal memory to the System 573
     * @param jvsInputBytes - Packet being read.
     * @param i - Current index of the parser.
     * @return new index of the parser.
     */
    private int whiteReadBuffer(short[] jvsInputBytes, int i)
    {
        //Read buffer from internal memory to System 573
        /*
         e0 01 07 70 00 02 04 00 80 fe
                   0  1  2  3  4  5  6
         e0 01 07 70 00 02 00 00 80 fa
         0 - Read Buffer
         1 - Read Buffer
         2 - Offset
         3 - Offset
         4 - Offset
         5 - Frame Count 
         6 - Checksum Byte
         */
         /*System.out.println("BUF -> 573 (70 00) "+ 
         "SrcPos1: "+(int)jvsInputBytes[i + 2]+", SrcPos2: "+(int)jvsInputBytes[i + 3]+", SrcPos3: "+(int)jvsInputBytes[i + 4]+
         ", SrcPosM: "+((int) jvsInputBytes[i + 2] * 0x10000 + (int) jvsInputBytes[i + 3] * 0x100 + (int) jvsInputBytes[i + 4]) +
         ", Len: "+(int)jvsInputBytes[i + 5]);//*/
        jvsOutputBytes[outputPosition] = 0x01;
        outputPosition++;
        outputBytesCount++;// 0x01
        outputBytesCount = outputBytesCount + (int) jvsInputBytes[i + 5];
        System.arraycopy(
                theBuffer, //Source
                ((int) jvsInputBytes[i + 2] * 0x10000 + (int) jvsInputBytes[i + 3] * 0x100 + (int) jvsInputBytes[i + 4]), //Source Pos
                jvsOutputBytes, //Dest
                outputPosition, //Dest Pos
                (int) jvsInputBytes[i + 5]); //Length
        //End Arraycopy
        outputPosition = outputPosition + (int) jvsInputBytes[i + 5];
        i = i + 6;
        return i;
    }
    /**
     * Write Internal WhitePCB buffer from the System 573
     * @param jvsInputBytes The JVS packet being worked on.
     * @param i The current index of the parser.
     * @return The new index of the parser.
     */
    private int whiteWriteBuffer(short[] jvsInputBytes, int i)
    {
        /*
         e0 01 87 70 01 02 00 00 80 
                   0  1  2  3  4  5
         0 - Write Buffer
         1 - Write Buffer
         2 - Offset 
         3 - Offset 
         4 - Offset -- Appears to Always be 0...
         5 - Length
         */
        /*
        System.out.println(
            "573 -> BUF (70 01) || "+
            "Offset1:"+(int)jvsInputBytes[i+2]+
            ", Offset2:"+(int)jvsInputBytes[i+3]+
            ", Offset3:"+(int)jvsInputBytes[i+4]+
            ", Offset ByteM "+((int) jvsInputBytes[i + 2] * 0x10000 + (int) jvsInputBytes[i + 3] * 0x100 + (int) jvsInputBytes[i + 4])+
            ", Length "+(int)jvsInputBytes[i+5]);
        //*/
        scribbleCheck = true;//If it's a scribble
        jvsOutputBytes[outputPosition] = 0x01;
        outputPosition++;
        outputBytesCount++;
        if (firstCommand71) //Skip Code Downloads
        {
            return i + 6 + (int)jvsInputBytes[i+5];
        }
        for (int j = i + 6; j < i + 16; j++) {
            if (jvsInputBytes[j] != (short) 0xFF) {
                scribbleCheck = false;
            }
        }
        //Don't buffer the Bytes
        if (scribbleCheck) 
        {
            //System.out.println("Scribble detected!");
        }
        //Buffer the bytes.
        else
        {
            System.arraycopy(jvsInputBytes,
                i + 6,
                theBuffer,
                ((int) jvsInputBytes[i + 2] * 0x10000 + (int) jvsInputBytes[i + 3] * 0x100 + (int) jvsInputBytes[i + 4]),
                (int) jvsInputBytes[i + 5]);
        }
        //End Arraycopy
        i = i + 6 + (int) jvsInputBytes[i + 5];
        return i;
    }
    /**
     * Sets the execution address for the WhitePCB This command is ignored by libJVS but we have to pretend we care...
     * @param jvsInputBytes The current packet being worked on.
     * @param i The current index of the parser
     * @return The new index of the parser
     */
    private int whiteSetExecutionAddress(short[] jvsInputBytes, int i)
    {
        jvsOutputBytes[outputPosition] = 0x01;
        outputPosition++;
        outputBytesCount++;
        i = i + 4;
        return i;
    }
    /**
     * This sets the flags for successful execution of a program downloaded to a WhitePCB
     * @param i The current index of the parser.
     * @return The new index of the parser.
     */
    private int whiteExecuteProgram(int i)
    {
        i = i + 1;
        jvsOutputBytes[outputPosition] = 0x01;
        outputPosition++;
        outputBytesCount++;
        flags = new short[]{0x00, 0x00};
        return i;
    }
    /**
     * Checks the flags of the WhitePCB and sends them back to the System 573
     * @param i current index of the parser. 
     * @return new index of the parser.
     */
    //WRONG: This function does a relaly terrible job here, make it work correctly.
    private int whiteGetFlags(int i)
    {
        outputPosition++;
        outputBytesCount = 5;//Wrong Implementation.
        jvsOutputBytes = new short[]{0xE0, 0x00, 0x05, 0x01, 0x01, flags[0], flags[1], 0x0f};//very WRONG
        if (firstCommand71)//Set Skip Code Downloads off.
        {
            firstCommand71 = false;
        }
        return i;
    }
    //The reason this code looks WRONG is because I think it might still be wrong. And Wrong code should look wrong.
    private int whiteReadCard(short[] jvsInputBytes, int i)
    {
        /* 76 74 Packet format:
         e0 01 0a 76 74 00 91 02 00 00 00 01 89
                   0  1  2  3  4  5  6  7  8  9
         e0 01 0a 76 74 00 00 02 00 00 00 10 07
        //e0 01 0a 76 74 00 42 02 00 00 00 01 3a                
         0 Read Card
         1 Read Card
         2 Address (+0x80 means Slot 2)
         3 Address
         4 Buffer Address
         5 Buffer Address
         6 Buffer Address
         7 Length (Blocks)
         8 Length (Blocks)
         9 Checksum
         */
        //e0 01 0a 76 74 00 42 02 00 00 00 01 3a
        if (jvsInputBytes[i + 2] < 0x80) 
        {
            selectedSlot = 0;
        }
        if (jvsInputBytes[i + 2] >= 0x80) 
        {
            selectedSlot = 1;
        }

        //Good for debugging if reading breaks
        /*if ((int)jvsInputBytes[i+8] != 1)
        {
            printJVSPacket(jvsInputBytes,false);
            System.out.println("Card"+(selectedSlot+1)+"Ready: "+MemoryCardManager.cardReady(selectedSlot));
        }//*/
        ////////////////////////////////////////////////
        // BIG IMPORTANT THING, SHOULD IMPLENT FLAG 8200 FOR CARD BUSY AND THEN FIX THE FLAG SETTING BELOW THIS
        ////////////////////////////////////////////////
        //flag 8200 for Busy card - not implemented. 
        //BrightWhite works around this by having data 
        //prepared prior to insert flag sent.
        if (MemoryCardManager.cardReady(selectedSlot)) 
        {
            /*System.out.println("CRD -> BUF (76 74) || Slot:"+(selectedSlot+1)+
                ", SrcPos1 " + (int)jvsInputBytes[i+2]+", SrcPos2 " + (int)jvsInputBytes[i+3] +
                ", SrcPOSM: " + (((int)jvsInputBytes[i+2]*0x100+(int)jvsInputBytes[i+3])*128) +
                ", Dest POS: " + ((int)jvsInputBytes[i+4]*0x10000+(int)jvsInputBytes[i+5]*0x100+(int)jvsInputBytes[i+6])+
                ", Len: " + (((int)jvsInputBytes[i+7]*0x100+(int)jvsInputBytes[i+8])*128));//*/
            //Set Flags to a success
            flags = new short[]{0x80, 0x00};
            /*int length = (((int) jvsInputBytes[i + 7] * 0x100 + (int) jvsInputBytes[i + 8]) * 128);
            int sourcePos = ((((int) jvsInputBytes[i + 2] - (int) jvsInputBytes[i + 2] & 0x80) * 0x100 + (int) jvsInputBytes[i + 3]) * 128);
            int destPos = ((int) jvsInputBytes[i + 4] * 0x10000 + (int) jvsInputBytes[i + 5] * 0x100 + (int) jvsInputBytes[i + 6]);*/
            MemoryCardManager.readCardAddress(selectedSlot,((((int) jvsInputBytes[i + 2] - (int) jvsInputBytes[i + 2] & 0x80) * 0x100 + (int) jvsInputBytes[i + 3]) * 128)); 
            //Copy the bytes from MemoryCardManager to the buffer
            System.arraycopy(
                MemoryCardManager.getCard(selectedSlot).getCard(), //src
                ((((int) jvsInputBytes[i + 2] & 127) * 0x100 + (int) jvsInputBytes[i + 3]) * 128), //spos
                theBuffer, //dest
                ((int) jvsInputBytes[i + 4] * 0x10000 + (int) jvsInputBytes[i + 5] * 0x100 + (int) jvsInputBytes[i + 6]), //dpos
                (((int) jvsInputBytes[i + 7] * 0x100 + (int) jvsInputBytes[i + 8]) * 128)); //len
            
           //*/
            /*System.arraycopy(
                MemoryCardManager.readCard(selectedSlot, sourcePos, length+1), //src
                0, //spos
                theBuffer, //dest
                destPos, //dpos
                length); //len//*/
            //End Arraycopy
        }
        //Card isn't fully ready to get the bytes from, report that it is busy to the System 573 instead.
        /*if (!MemoryCardManager.cardReady(selectedSlot))// && MemoryCardManager.cardInserted(selectedSlot))
        {
            flags=new short[]{0x82, 0x00};
        }*/
        //That slot is empty. Tell the 573
        if (!MemoryCardManager.cardInserted(selectedSlot) && !MemoryCardManager.cardReady(selectedSlot)) 
        {
            flags = new short[]{0x00, 0x08};
        }
        //additional magic required
        jvsOutputBytes[outputPosition] = 0x01;
        jvsOutputBytes[outputPosition + 1] = 0x01;
        outputPosition = outputPosition + 2;
        outputBytesCount = outputBytesCount + 2;
        i = i + 10;
        return i;
    }
    /**
     * WhitePCB Buffer to Memory card Write command
     * Issues write request to the MemoryCardManager 
     * @param jvsInputBytes Packet being worked on
     * @param i Current index of the parser
     * @return New Index of the parser.
     */
    private int whiteWriteCard(short[] jvsInputBytes, int i)
    {
        /*
        e0 01 0a 76 75 02 00 00 00 b7 00 01 b0
                  0  1  2  3  4  5  6  7  8  9
        0 - Write Card
        1 - Write Card
        2 - Buffer Address
        3 - Buffer Address
        4 - Buffer Address
        5 - Probably Also Offset - Add 0x80 for Slot 2?
        6 - Offset / 128
        7 - Length / 128
        8 - Length / 128
        9 - JVS Checksum
        */
        outputBytesCount = 4;
        flags = new short[]{0x80, 0x00}; // Should be set by Card ready status.
        jvsOutputBytes = new short[]{0xe0, 0x00, 0x04, 0x01, 0x01, 0x01, 0x07}; //Very, Very wrong.
        //Copy to buffer here!
        if (scribbleCheck) 
        {
            //If you Scribble at me, Report that I did the right thing and Ignored it.
            flags = new short[]{0x00, 0x00};
        } 
        else 
        {
            /*System.out.println("BUF -> CRD (76 75) || Slot:"+(selectedSlot+1)+
                ", BufPos1 "+(int)jvsInputBytes[i+2]+", BufPos2 "+(int)jvsInputBytes[i+3]+", BufPos3 " +(int)jvsInputBytes[i+4]+
                ", BufPosM: "+((int)jvsInputBytes[i+2]*0x10000+(int)jvsInputBytes[i+3]*0x100+(int)jvsInputBytes[i+4])+
                ", Dest Slot: "+((int)jvsInputBytes[i+5]&0x80) +
                ", Dest POS: "+(((int)jvsInputBytes[i+5]-(int)jvsInputBytes[i+5]&0x80)*0x100+(int)jvsInputBytes[i+6])+
                ", Len: " + (((int)jvsInputBytes[i+7]*0x100+(int)jvsInputBytes[i+8])*128));
            //*/
            if (jvsInputBytes[i+5] < 0x80) 
            {
                selectedSlot=0;
            }
            if (jvsInputBytes[i+5] >= 0x80) 
            {
                selectedSlot=1;
            }
            //Possibly ditch temp?
            temp = new short[(((int)jvsInputBytes[i+7]*0x100+(int)jvsInputBytes[i+8])*128)];
            /*System.arraycopy(
                theBuffer,
                (((int)jvsInputBytes[i+2]*0x10000+(int)jvsInputBytes[i+3]*0x100+(int)jvsInputBytes[i+4])),
                temp,
                0,
                (((int)jvsInputBytes[i+7]*0x100+(int)jvsInputBytes[i+8])*128));//*/
            int bufferOffset=((jvsInputBytes[i+2]*0x10000+jvsInputBytes[i+3]*0x100+jvsInputBytes[i+4]));
            int writeLength=((jvsInputBytes[i+7]*0x100+jvsInputBytes[i+8])*128);
            int cardOffset=(((jvsInputBytes[i+5]-jvsInputBytes[i+5]&0x80)*0x100+jvsInputBytes[i+6])*128);
            
            //I should check <crossout>myself</crossout>the result before I wreck <crossout>myself</crossout> the result.
            MemoryCardManager.writeCard(
                selectedSlot, 
                Arrays.copyOfRange(theBuffer,bufferOffset,(bufferOffset+writeLength)),
                cardOffset,
                writeLength);
            //set flags */
            flags = new short[]{0x80, 0x00};
        }
        i = i + 9;
        return i;
    }
    /**
     * Handles JVS Broadcast packets. 
     * @param jvsInputBytes the JVS packet to be worked on
     */
    private void jvsBroadcast(short[] jvsInputBytes)
    {
        if (jvsInputBytes[2] == 0x03 && jvsInputBytes[3] == 0xF0 && jvsInputBytes[4] == 0xD9 && jvsInputBytes[5] == 0xCB) 
        {
            System.out.println("JVS Reset Detected");
            senseStatus = true;
            jvsAddress = -1;//Address no longer valid, -1 cannot be a JVS address
            jvsWrite = false;//No Write this turn
            jvsInitialized = false;//Not Initialized - skip other functions
            jvsInputBytes = new short[256];
            if (setType == TYPE_WHITEPCB)
            {
                firstCommand71 = true; //Reset 
            }
        }
        //JVS Address Packet...
        //Accept if Sense is Low, Otherwise ignore.
        //  0   1   2   3   4   5
        //[E0][FF][03][F1][01][F4]
        if (jvsInputBytes[2] == 0x03 && jvsInputBytes[3] == 0xF1) 
        {
            //inputSense.isLow() &&
            if (jvsChecksum) 
            {
                jvsAddress = (jvsInputBytes[4]);
                senseStatus = false;
                //senseGoLow = true;
                jvsWrite = true;
                jvsOutputBytes = new short[]{0xe0, 0x00, 0x03, 0x01, (short)(jvsAddress & 0xFF), 0x00};
                jvsOutputBytes[5] = jvsChecksum(jvsOutputBytes);
                jvsInitialized = true;
                System.out.println("JVS Address accepted");
            }
        }
    }
    
    private class jvsEvent implements jvsEventListener
    {
        @Override
        public void packetRecieved() 
        {
            process();
        }
    }
}