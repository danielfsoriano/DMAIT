/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package logparser;

import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author daniel
 */
public class MoteConversation {
    private String dmText = null;
    private String dmTimestamp = null;
    private String dmDestinationAddr = null;
    private String dmHmac = null;
    private boolean dmReceived = false;
    private String dmReceptionTimestamp = null;

    private String ackText = null;

    private String ackTimestamp = null;

    private boolean ackReceived = false;
    private String ackReceptionTimestamp = null;


    public String getDmText() {
        return dmText;
    }

    public void setDmText(String dmText) {
        this.dmText = dmText;
    }

    public String getDmReceptionTimestamp() {
        return dmReceptionTimestamp;
    }

    public void setDmReceptionTimestamp(String dmReceptionTimestamp) {
        if (dmReceptionTimestamp != null) {
            this.dmReceived = true;
            this.dmReceptionTimestamp = dmReceptionTimestamp;
        }        
    }

    public boolean isDmReceived() {
        return this.dmReceived;
    }

    public String getDmHmac() {
        return this.dmHmac;
    }

    public void setDmHmac(String hmac) {
        this.dmHmac = hmac;
    }

    public String getDmDestinationAddr() {
        return this.dmDestinationAddr;
    }

    public void setDmDestinationAddr(String dmDestinationAddr) {
        this.dmDestinationAddr = dmDestinationAddr;
    }

    public String getDmTimestamp() {
        return this.dmTimestamp;
    }

    public void setDmTimestamp(String timestamp) {
        this.dmTimestamp = timestamp;
    }

    public String getAckText() {
        return ackText;
    }

    public void setAckText(String ackText) {
        this.ackText = ackText;
    }

    public String getAckTimestamp() {
        return ackTimestamp;
    }

    public void setAckTimestamp(String ackTimestamp) {
        this.ackTimestamp = ackTimestamp;
    }

    public boolean isAckReceived() {
        return ackReceived;
    }


    public String getAckReceptionTimestamp() {
        return ackReceptionTimestamp;
    }

    public void setAckReceptionTimestamp(String ackReceptionTimestamp) {
        if (ackReceptionTimestamp != null) {
            this.ackReceived = true;
            this.ackReceptionTimestamp = ackReceptionTimestamp;
        }
    }

    public Long calculateDmDelay() {
        Long result = null;
        if (this.isDmReceived()) {
            result = calculateDelay(this.dmTimestamp, this.dmReceptionTimestamp);
        }
        return result;
    }

    public Long calculateAckDelay() {
        Long result = null;
        if (this.isAckReceived()) {
            result = calculateDelay(this.ackTimestamp, this.ackReceptionTimestamp);
        }
        return result;
    }

    private Long calculateDelay(String sentTime, String receivedTime) {
        Long result = null;
        try {
            Date sent = MoteConversation.parseTime(sentTime);
            Date recv = MoteConversation.parseTime(receivedTime);

            result = recv.getTime() - sent.getTime();
        } catch (ParseException ex) {
            Logger.getLogger(MoteConversation.class.getName()).log(Level.SEVERE, null, ex);
        }
        return result;
    }


    public static Date parseTime(String time) throws ParseException {
        //System.out.println("TIME: " + time);
        int pointIndex = time.indexOf(".");
        int sentMinutes = 0;
        int sentSeconds = 0;

        if (pointIndex >= 0) {
            String sentSecondsAux = time.substring(0, pointIndex);
            sentSeconds = Integer.parseInt(sentSecondsAux);
        }

        if (sentSeconds > 60) {
            sentMinutes = sentSeconds / 60;
            sentSeconds = sentSeconds % 60;
        }
        String sentMilliAux = time.substring(pointIndex+1);
        if (sentMilliAux.length() < 3) {
            sentMilliAux = sentMilliAux + "000";
        }
        if (sentMilliAux.length() > 3) {
            sentMilliAux = sentMilliAux.substring(0, 3);
        }

        String timeStr = String.format("%02d", sentMinutes) +":"+ String.format("%02d", sentSeconds) +"."+ sentMilliAux;
        DateFormat df = new SimpleDateFormat("mm:ss.SSS");
        return df.parse(timeStr);
    }


    public String toString() {
/*        
    private String timestamp = null;
    private int destinationId = 0;
    private boolean answered = false;
    private String answerTimestamp = null;
    private String message = null;        
*/        
        StringBuilder sb = new StringBuilder();
        sb.append("Data Message: ");
        sb.append(this.dmText);
        sb.append(", HMAC: ");
        sb.append(this.dmHmac);
        sb.append(", Para: ");
        sb.append(this.dmDestinationAddr);
        sb.append(", Enviada: ");
        sb.append(this.dmTimestamp);
        sb.append(", Recebida: ");
        if (dmReceived) {
            sb.append(this.dmReceptionTimestamp);
            sb.append(", DELAY: ");
            sb.append(calculateDmDelay() + " ms");

            sb.append(", ACK: ");
            sb.append(this.ackText);
            sb.append(", Enviado: ");
            sb.append(this.ackTimestamp);
            sb.append(", Recebido: ");
            if (this.ackReceived) {
                sb.append(this.ackReceptionTimestamp);
                sb.append(", ACK_DELAY: ");
                sb.append(calculateAckDelay() + " ms");
            } else {
                sb.append("ACK NAO RECEBIDO");
            }

        } else {
            sb.append("NAO RECEBIDA");
        }

                
        return sb.toString();        
    }
    
    
}
