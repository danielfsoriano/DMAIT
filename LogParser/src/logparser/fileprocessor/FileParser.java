/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package logparser.fileprocessor;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.math.BigDecimal;
import java.math.RoundingMode;
import java.text.DecimalFormat;
import java.util.Date;
import java.util.StringTokenizer;

import logparser.HexUtils;
import logparser.MoteConversation;
import logparser.MoteData;


/**
 *
 * @author Administrator
 */
public class FileParser {

    private File file = null;
    private RandomAccessFile raf = null;

    private Integer startDataLine;

    private MoteData moteData = null;

    private String attackStartTimestamp = null;
    private String firstLocalReactionTimestamp = null;
    private String lastLocalReactionStartTimestamp = null;
    private String firstAttackNotificationReceivedByRootTimestamp = null;

    /*
        DMAIT payload header names
        #define DMAIT_ROUTE_VALIDATION_HEADER "RV".............OK
        #define DMAIT_ROUTE_VALIDATION_ACK_HEADER "RK".........OK
        #define DMAIT_ALTERNATE_ROUTE_HEADER "AR"..............OK
        #define DMAIT_ALTERNATE_ROUTE_ACK_HEADER "AK"..........NOK (MENSAGEM NAO USADA NO DMAIT)
        #define DMAIT_FAULT_LOCATION_PROBE_HEADER "FL".........OK
        #define DMAIT_FAULT_LOCATION_ACK_HEADER "FK"...........OK
        #define DMAIT_DATA_MESSAGE_HEADER "DM".................OK
        #define DMAIT_DATA_MESSAGE_ACK_HEADER "DK".............OK
     */


    private int counterDataMessages = 0;
    private int counterDataAckMessages = 0;

    private int counterControlRV = 0;
    private int counterControlRK = 0;
    private int counterControlAR = 0;
    private int counterControlFL = 0;
    private int counterControlFK = 0;



    public FileParser(File file, MoteData moteData) throws Exception {
        setFile(file);
        setMoteData(moteData);
    }

    public FileParser(String fileName, MoteData moteData) throws Exception {
        setFile(new File(fileName));
        setMoteData(moteData);        
    }
    
    public final void setFile(File file) throws Exception {
        this.file = file;
    }
      
    public File getFile(){
        return this.file;
    }
    
    private void setMoteData(MoteData moteData) {
        this.moteData = moteData;
    }

    public MoteData getMoteData() {
        return moteData;
    }
    
    private void addMoteConversation(int moteId, MoteConversation mc) {
        this.moteData.addMoteConversation(moteId, mc);
    }

    private void setMoteParent(int modeId, String parent) {
        this.moteData.setMoteParent(modeId, parent);
    }

    private void addAtacker(int moteId) {
        this.moteData.addAtacker(moteId);
    }

    public final void processFile() throws Exception {
        if (this.file != null){
            this.raf = new RandomAccessFile(this.file, "r");
            this.parseFile();
        }
    }



    public RandomAccessFile getRandomAccessFile() {
        return this.raf;
    }

    private void initialize() {

    }

 
    private void parseFile() throws Exception, IOException{
        if (this.raf != null) {
            this.raf.seek(0);

            try {
                String line = null;
                int lineNumber=1;
                initialize();

                boolean attackStarted = false;

                while ((line = this.raf.readLine()) != null) {

                    //line = filterUnwantedChars(line); //remove tabs

                    //empty -> DISCARD
                    if (!isEmptyLine(line)) { //only processes lines that are not empty

                        //counters
                        if (lineContains(line, "ENVIANDO TEXTO: \"RV")) {
                            this.counterControlRV++;
                        } else if (lineContains(line, "ENVIANDO TEXTO: \"RK")) {
                            this.counterControlRK++;
                        } else if (lineContains(line, "ENVIANDO TEXTO: \"AR")) {
                            this.counterControlAR++;
                        } else if (lineContains(line, "ENVIANDO TEXTO: \"FL")) {
                            this.counterControlFL++;
                        } else if (lineContains(line, "ENVIANDO TEXTO: \"FK")) {
                            this.counterControlFK++;
                        } else if (lineContains(line, "ENVIANDO TEXTO: \"DM")) {
                            this.counterDataMessages++;
                        } else if (lineContains(line, "ENVIANDO TEXTO: \"DK")) {
                            this.counterDataAckMessages++;
                        }

                        //Message processing
                        if (this.firstAttackNotificationReceivedByRootTimestamp == null && isRootNotificationReactionLine(line)) {
                            String timestamp = this.getLineTimestamp(line);
                            this.firstAttackNotificationReceivedByRootTimestamp = timestamp;

                            //this.processRootAtackNotificationReactionLine(line, raf);
                        } else if (isAttackStartLine(line)) {
                            attackStarted = true;
                            this.processAttackStartLine(line, raf);

                        } else if (!attackStarted && isParentLine(line)) {
                            this.processParentLine(line, raf);

                        } else if (isDMAITARInitLine(line)) {
                            String timestamp = this.getLineTimestamp(line);
                            if (this.firstLocalReactionTimestamp == null) {
                                this.firstLocalReactionTimestamp = timestamp;
                            }
                            this.lastLocalReactionStartTimestamp = timestamp;

                        } else if (isDataSendLine(line)) {
                            this.processDataSendLine(line, raf);
                        }
                    }
                    lineNumber++;
                    //System.out.println("PROCESSANDO LINHA: " + lineNumber);
                }
                System.out.println("\nARQUIVO CONCLUIDO\n");
                String attackStartedAt = "null";
                if (this.attackStartTimestamp != null) {
                    attackStartedAt = this.attackStartTimestamp.toString().replace(".", ",");
                }
                String lastReactionAt = "null";
                if (this.lastLocalReactionStartTimestamp != null) {
                    lastReactionAt = this.lastLocalReactionStartTimestamp.toString().replace(".", ",");
                }
                System.out.println("ATTACK STARTED AT: " + attackStartedAt);
                System.out.println("LAST REACTION AT: " +  lastReactionAt);

                if (this.attackStartTimestamp != null && this.lastLocalReactionStartTimestamp != null) {
                    BigDecimal start = new BigDecimal(MoteConversation.parseTime(this.attackStartTimestamp).getTime());
                    BigDecimal end = new BigDecimal(MoteConversation.parseTime(this.lastLocalReactionStartTimestamp).getTime());
                    BigDecimal delta = end.subtract(start).divide(new BigDecimal(1000));

                    System.out.println("    NETWORK CONVERGENCE TIME: " + delta.toString().replace(".", ",") + "\n\n");
                }

                String firstLocalReactionAt = "null";
                if (this.firstLocalReactionTimestamp != null) {
                    firstLocalReactionAt = this.firstLocalReactionTimestamp.toString().replace(".", ",");
                }
                String firtRootNotificationReceivedAt = "null";
                if (this.firstAttackNotificationReceivedByRootTimestamp != null) {
                    firtRootNotificationReceivedAt = this.firstAttackNotificationReceivedByRootTimestamp.toString().replace(".", ",");
                }
                System.out.println("FIRST LOCAL REACTION AT: " +  firstLocalReactionAt);
                System.out.println("FIRST ROOT NOTIFICATION RECEIVED AT: " + firtRootNotificationReceivedAt);
                if (this.attackStartTimestamp != null && this.firstLocalReactionTimestamp != null && this.firstAttackNotificationReceivedByRootTimestamp != null) {
                    BigDecimal start = new BigDecimal(MoteConversation.parseTime(this.attackStartTimestamp).getTime());
                    BigDecimal localReaction = new BigDecimal(MoteConversation.parseTime(this.firstLocalReactionTimestamp).getTime());
                    BigDecimal globalReaction = new BigDecimal(MoteConversation.parseTime(this.firstAttackNotificationReceivedByRootTimestamp).getTime());
                    BigDecimal localReactionTime = localReaction.subtract(start).divide(new BigDecimal(1000));
                    BigDecimal globalReactionTime = globalReaction.subtract(start).divide(new BigDecimal(1000));

                    System.out.println("    FIRST LOCAL REACTION TIME: " + localReactionTime.toString().replace(".", ","));
                    System.out.println("    FIRST GLOBAL REACTION TIME: " + globalReactionTime.toString().replace(".", ","));
                }

                System.out.println("\n\n");

                final DecimalFormat df = new DecimalFormat("0.00");

                Double ackPercentIncrease = ((double)this.counterDataAckMessages / (double) this.counterDataMessages) * 100d;
                int controlMessagesTotal = (this.counterControlRV + this.counterControlRK + this.counterControlAR + this.counterControlFL + this.counterControlFK);
                Double controlMessagePercentIncrease = ((double) controlMessagesTotal / (double) (this.counterDataMessages + this.counterDataAckMessages) * 100d);


                System.out.println("==== COUNTERS");
                System.out.println("DATA MESSAGES: " + this.counterDataMessages);
                System.out.println("DATA ACKS....: " + this.counterDataAckMessages + " (+" + df.format(ackPercentIncrease) + "%, over DATA messages only)");
                System.out.println("CONTROL MESSAGES: " + controlMessagesTotal + " (+" + df.format(controlMessagePercentIncrease) + "%, over DATA + ACKS messages)");
                System.out.println("    RVs..: " + this.counterControlRV);
                System.out.println("    RKs..: " + this.counterControlRK);
                System.out.println("    ARs..: " + this.counterControlAR);
                System.out.println("    FLs..: " + this.counterControlFL);
                System.out.println("    FKs..: " + this.counterControlFK);

                System.out.println("\n\n");

            } catch (FileNotFoundException ex) {
                throw new Exception(ex);
            } catch (IOException ex) {
                throw new Exception(ex);
            }
        }
    }

/*
81.003	ID:6	[INFO: DMAIT     ] ENVIANDO MENSAGEM DE TEXTO: "DM-0001-018-TESTE MSG DE DADOS;8f2f0c3fba;", PARA: fd00::201:1:1:1
 */
    private boolean isDataSendLine(String line) {
        if (line.trim().contains("ENVIANDO TEXTO: \"DM")) {
            return true;
        }
        return false;
    }

/*
300.03	ID:2	[WARN: SFA_on ] attack has started
 */
    private boolean isAttackStartLine(String line) {
        if (line.trim().contains("attack has started")) {
            return true;
        }
        return false;
    }

/*
37.597	ID:16	[INFO: App       ] PAI ATUAL: fe80::20e:e:e:e, (LLADDR): 000e.000e.000e.000e
 */
    private boolean isParentLine(String line) {
        if (line.trim().contains("PAI ATUAL:")) {
            return true;
        }
        return false;
    }


/*
362.885	ID:4	[INFO: DMAIT     ] INICIANDO DMAIT-AR
 */
    private boolean isDMAITARInitLine(String line) {
        if (line.trim().contains("INICIANDO DMAIT-AR")) {
            return true;
        }
        return false;
    }

/*
386.410544	ID:1	NODE SUSPICION HAS BEEN INCREASED! - 0202:0002:0002:0002 - NEGATIVE: 2, POSITIVE: 1, DIFFERENCE: 1, RATIO: 0.50
 */
    private boolean isRootNotificationReactionLine(String line) {
        if (line.trim().contains("NODE SUSPICION HAS BEEN INCREASED!")) {
            return true;
        }
        return false;
    }


    private boolean lineContains(String line, String text) {
        if (line.trim().contains(text)) {
            return true;
        }
        return false;
    }

/*
81.009688	ID:1	[INFO: App       ] MENSAGEM RECEBIDA: "DM-0001-018-TESTE MSG DE DADOS;8f2f0c3fba;", DE: fd00::206:6:6:6
 */
    private boolean isDataRecvLine(String line) {
        if (line.trim().contains("RECEBIDA: \"DM")) {
            return true;
        }
        return false;
    }

    private boolean isDataRecvLine(String line, String hmac) {
        if (line.trim().contains("RECEBIDA: \"DM") && line.trim().contains(hmac)) {
            return true;
        }
        return false;
    }

    private boolean isAckSentLine(String line, String hmac) {
        if (line.trim().contains("ENVIANDO TEXTO: \"DK") && line.trim().contains(hmac)) {
            return true;
        }
        return false;
    }

    private boolean isAckReceivedLine(String line, String hmac) {
        if (line.trim().contains("RECEBIDA: \"DK") && line.trim().contains(hmac)) {
            return true;
        }
        return false;
    }

    private String filterUnwantedChars(String text) {
        //replaces the TAB character with the space
        //return text.replace("\t", " ");
        return text;
    }

    private boolean isEmptyLine(String line) {
        if (line == null || line.trim().length() == 0) {
            return true;
        }
        return false;
    }

/*
300.03	ID:2	[WARN: SFA_on ] attack has started
 */
    private void processAttackStartLine(String line, RandomAccessFile raf) throws Exception {

        StringTokenizer st = new StringTokenizer(line, "\t");

        String timestamp = st.nextToken();
        if (!timestamp.contains(".")) {
            timestamp = timestamp + ".0";
        }
        if (this.attackStartTimestamp == null) {
            this.attackStartTimestamp = timestamp; //...get the first attack time
        }

        int moteId = Integer.valueOf(st.nextToken().trim().replaceAll("ID:", "")).intValue();
        this.addAtacker(moteId);
    }

/*
406.576	ID:3	[INFO: DMAIT     ] INICIANDO DMAIT-AR
 */
    private String getLineTimestamp(String line) throws Exception {

        StringTokenizer st = new StringTokenizer(line, "\t");

        String timestamp = st.nextToken();
        if (!timestamp.contains(".")) {
            timestamp = timestamp + ".0";
        }
        return timestamp;
    }

/*
37.597	ID:16	[INFO: App       ] PAI ATUAL: fe80::20e:e:e:e, (LLADDR): 000e.000e.000e.000e
 */
    private void processParentLine(String line, RandomAccessFile raf) throws Exception {

        StringTokenizer st = new StringTokenizer(line, "\t");

        String timestamp = st.nextToken();
        if (!timestamp.contains(".")) {
            timestamp = timestamp + ".0";
        }
        int moteId = Integer.valueOf(st.nextToken().trim().replaceAll("ID:", "")).intValue();
        String text = st.nextToken();

        int lladdrStart = text.lastIndexOf(":");
        String lladdr = text.substring(lladdrStart+1);

        setMoteParent(moteId, lladdr);
    }



/*
386.410544	ID:1	NODE SUSPICION HAS BEEN INCREASED!...
 */
/*
    private void processRootAtackNotificationReactionLine(String line, RandomAccessFile raf) throws Exception {

    StringTokenizer st = new StringTokenizer(line, "\t");

    String timestamp = st.nextToken();
    if (!timestamp.contains(".")) {
        timestamp = timestamp + ".0";
    }
    this.firstAttackNotificationReceivedByRootTimestamp = timestamp;
}
*/

/*
81.003	ID:6	[INFO: DMAIT     ] ENVIANDO MENSAGEM DE DADOS: DM-0001-018-TESTE MSG DE DADOS;8f2f0c3fba;
81.003	ID:6	[INFO: DMAIT     ] ENVIANDO MENSAGEM DE TEXTO: "DM-0001-018-TESTE MSG DE DADOS;8f2f0c3fba;", PARA: fd00::201:1:1:1
*/
    private void processDataSendLine(String line, RandomAccessFile raf) throws Exception {
                
        StringTokenizer st = new StringTokenizer(line, "\t");         
        
        String timestamp = st.nextToken();
        if (!timestamp.contains(".")) {
            timestamp = timestamp + ".0";
        }
        int moteId = Integer.valueOf(st.nextToken().trim().replaceAll("ID:", "")).intValue();
        String text = st.nextToken();
        
        int messageStart = text.indexOf("\"");
        int messageEnd   = text.indexOf("\"", messageStart + 1);
        String message = text.substring(messageStart, messageEnd).replace("\"", "");

        int hmacStart = message.indexOf(";");
        String hmac = message.substring(hmacStart);
        hmac = hmac.replace(";", "");

        String destinationAUX = text.substring(messageEnd).replace(", PARA: ", "").trim();
        //destinationAUX = destinationAUX.substring(0, destinationAUX.indexOf("'")).trim();
        destinationAUX = destinationAUX.substring(destinationAUX.lastIndexOf(":"));
        destinationAUX = destinationAUX.replace(":", "").trim();
        //int destination = Integer.valueOf(destinationAUX);
        int destination = HexUtils.getDecimal(destinationAUX);
        
        MoteConversation mc = new MoteConversation();
        mc.setDmTimestamp(timestamp);
        mc.setDmDestinationAddr(destinationAUX);
        mc.setDmHmac(hmac);
        mc.setDmText(message);
        mc.setDmReceptionTimestamp(getAnsweredTime(message, moteId, destination, raf, hmac));
        mc.setAckText(getAckMessage(raf, hmac));
        mc.setAckTimestamp(getAckSentTime(raf, hmac));
        mc.setAckReceptionTimestamp(getAckReceivedTime(raf, hmac));

        
        addMoteConversation(moteId, mc);

    }



/*
81.009688	ID:1	[INFO: App       ] MENSAGEM RECEBIDA: "DM-0001-018-TESTE MSG DE DADOS;8f2f0c3fba;", DE: fd00::206:6:6:6
*/
    private String getAnsweredTime(String message, int sourceMote, int targetMote, RandomAccessFile raf, String hmac) throws IOException {
        String result = null;
        if (raf != null) {
            long startFilePointer = this.raf.getFilePointer();

            RandomAccessFile nRaf = new RandomAccessFile(this.file, "r");
            nRaf.seek(startFilePointer);

            String line = null;
            while (result == null && (line = nRaf.readLine()) != null) {
                
                if (isDataRecvLine(line, hmac)) {
                    StringTokenizer st = new StringTokenizer(line, "\t");         

                    String timestamp = st.nextToken();        
                    int targetId = Integer.valueOf(st.nextToken().trim().replaceAll("ID:", "")).intValue();
                    String text = st.nextToken();                                        
                    
                    int sourceIdStart = text.lastIndexOf(":")+1;

                    String sourceIdAux = text.substring(sourceIdStart).trim();
                    int sourceId = HexUtils.getDecimal(sourceIdAux);

                    if (sourceMote == sourceId && targetMote == targetId && text.contains(message)) {
                        //found the answer
                        result = timestamp;
                    }
                }
            }              
            //get the file position back
            this.raf.seek(startFilePointer);
            nRaf.close();
        }                
        return result;        
    }

/*
74.132688	ID:1	[INFO: DMAIT     ] ENVIANDO MENSAGEM DE TEXTO: "DK-0001;3770db2ce0;adb3e6ef0b;", PARA: fd00::209:9:9:9
*/
    private String getAckMessage(RandomAccessFile raf, String hmac) throws IOException {
        String result = null;
        if (raf != null) {
            long startFilePointer = this.raf.getFilePointer();

            RandomAccessFile nRaf = new RandomAccessFile(this.file, "r");
            nRaf.seek(startFilePointer);

            String line = null;
            while (result == null && (line = nRaf.readLine()) != null) {

                if (isAckReceivedLine(line, hmac)) {
                    StringTokenizer st = new StringTokenizer(line, "\"");
                    st.nextToken();// discard first token
                    result = st.nextToken();
                }

            }
            //get the file position back
            this.raf.seek(startFilePointer);
            nRaf.close();
        }
        return result;
    }


/*
74.132688	ID:1	[INFO: DMAIT     ] ENVIANDO MENSAGEM DE TEXTO: "DK-0001;3770db2ce0;adb3e6ef0b;", PARA: fd00::209:9:9:9
*/
    private String getAckSentTime(RandomAccessFile raf, String hmac) throws IOException {
        String result = null;
        if (raf != null) {
            long startFilePointer = this.raf.getFilePointer();

            RandomAccessFile nRaf = new RandomAccessFile(this.file, "r");
            nRaf.seek(startFilePointer);

            String line = null;
            while (result == null && (line = nRaf.readLine()) != null) {

                if (isAckSentLine(line, hmac)) {
                    StringTokenizer st = new StringTokenizer(line, "\t");

                    result = st.nextToken();

                }
            }
            //get the file position back
            this.raf.seek(startFilePointer);
            nRaf.close();
        }
        return result;
    }
/*
74.157304	ID:9	[INFO: App       ] MENSAGEM RECEBIDA: "DK-0001;3770db2ce0;adb3e6ef0b;", DE: fd00::201:1:1:1
*/
    private String getAckReceivedTime(RandomAccessFile raf, String hmac) throws IOException {
        String result = null;
        if (raf != null) {
            long startFilePointer = this.raf.getFilePointer();

            RandomAccessFile nRaf = new RandomAccessFile(this.file, "r");
            nRaf.seek(startFilePointer);

            String line = null;
            while (result == null && (line = nRaf.readLine()) != null) {

                if (isAckReceivedLine(line, hmac)) {
                    StringTokenizer st = new StringTokenizer(line, "\t");

                    result = st.nextToken();
                }
            }
            //get the file position back
            this.raf.seek(startFilePointer);
            nRaf.close();
        }
        return result;
    }

 
    protected void finalize() throws Throwable {
        this.raf.close();
    }



}
