/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package logparser;

import java.io.File;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Set;
import logparser.fileprocessor.FileParser;

/**
 *
 * @author daniel
 */
public class LogParser {
    
    public static final String ARGUMENT_VALUE_DELIMITER = "=";
    public static final String LOG_FILENAME_ARGUMENT = "log";

    public static MoteData moteData = null;
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws Exception {
        ArrayList<String> logFileNames = null;

        HashMap<String, String> tokenizedArgs = LogParser.tokenizeArgs(args);        
/*        
        if (tokenizedArgs.containsKey(LogParser.LOG_FILENAME_ARGUMENT)) {
            logFileName = tokenizedArgs.get(LogParser.LOG_FILENAME_ARGUMENT);            
        }
*/
        Set keySet = tokenizedArgs.keySet();
        if (!keySet.isEmpty()) {
            logFileNames = new ArrayList<String>();
            logFileNames.addAll(keySet);
        }

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE/TEST1-16x2-5TREE-BASELINE-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE/TEST1-16x2-5TREE-BASELINE-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE/TEST1-16x2-5TREE-BASELINE-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE/TEST1-16x2-5TREE-BASELINE-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE/TEST1-16x2-5TREE-BASELINE-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE/TEST1-16x2-5TREE-BASELINE-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE/TEST1-16x2-5TREE-BASELINE-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE/TEST1-16x2-5TREE-BASELINE-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE/TEST1-16x2-5TREE-BASELINE-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE/TEST1-16x2-5TREE-BASELINE-SEED10-log.txt";

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/DMAIT/TEST1-16x2-5TREE-DMAIT-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/DMAIT/TEST1-16x2-5TREE-DMAIT-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/DMAIT/TEST1-16x2-5TREE-DMAIT-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/DMAIT/TEST1-16x2-5TREE-DMAIT-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/DMAIT/TEST1-16x2-5TREE-DMAIT-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/DMAIT/TEST1-16x2-5TREE-DMAIT-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/DMAIT/TEST1-16x2-5TREE-DMAIT-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/DMAIT/TEST1-16x2-5TREE-DMAIT-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/DMAIT/TEST1-16x2-5TREE-DMAIT-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/DMAIT/TEST1-16x2-5TREE-DMAIT-SEED10-log.txt";

//NOACKS

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-BASELINE-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-BASELINE-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-BASELINE-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-BASELINE-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-BASELINE-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-BASELINE-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-BASELINE-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-BASELINE-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-BASELINE-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-BASELINE-SEED10-log.txt";

//NOATTACK

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE/TEST1-16x2-5TREE-NOATTACK-BASELINE-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE/TEST1-16x2-5TREE-NOATTACK-BASELINE-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE/TEST1-16x2-5TREE-NOATTACK-BASELINE-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE/TEST1-16x2-5TREE-NOATTACK-BASELINE-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE/TEST1-16x2-5TREE-NOATTACK-BASELINE-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE/TEST1-16x2-5TREE-NOATTACK-BASELINE-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE/TEST1-16x2-5TREE-NOATTACK-BASELINE-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE/TEST1-16x2-5TREE-NOATTACK-BASELINE-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE/TEST1-16x2-5TREE-NOATTACK-BASELINE-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE/TEST1-16x2-5TREE-NOATTACK-BASELINE-SEED10-log.txt";

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/DMAIT/TEST1-16x2-5TREE-NOATTACK-DMAIT-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/DMAIT/TEST1-16x2-5TREE-NOATTACK-DMAIT-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/DMAIT/TEST1-16x2-5TREE-NOATTACK-DMAIT-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/DMAIT/TEST1-16x2-5TREE-NOATTACK-DMAIT-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/DMAIT/TEST1-16x2-5TREE-NOATTACK-DMAIT-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/DMAIT/TEST1-16x2-5TREE-NOATTACK-DMAIT-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/DMAIT/TEST1-16x2-5TREE-NOATTACK-DMAIT-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/DMAIT/TEST1-16x2-5TREE-NOATTACK-DMAIT-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/DMAIT/TEST1-16x2-5TREE-NOATTACK-DMAIT-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/DMAIT/TEST1-16x2-5TREE-NOATTACK-DMAIT-SEED10-log.txt";

//NOACKS

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-NOATTACK-BASELINE-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-NOATTACK-BASELINE-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-NOATTACK-BASELINE-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-NOATTACK-BASELINE-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-NOATTACK-BASELINE-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-NOATTACK-BASELINE-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-NOATTACK-BASELINE-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-NOATTACK-BASELINE-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-NOATTACK-BASELINE-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-5TREE-NOATTACK/BASELINE-NOACKS/TEST1-16x2-5TREE-NOACKS-NOATTACK-BASELINE-SEED10-log.txt";

///////

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM/BASELINE/TEST1-16x2-RANDOM-BASELINE-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM/BASELINE/TEST1-16x2-RANDOM-BASELINE-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM/BASELINE/TEST1-16x2-RANDOM-BASELINE-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM/BASELINE/TEST1-16x2-RANDOM-BASELINE-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM/BASELINE/TEST1-16x2-RANDOM-BASELINE-SEED5-log.txt";

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM/DMAIT/TEST1-16x2-RANDOM-DMAIT-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM/DMAIT/TEST1-16x2-RANDOM-DMAIT-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM/DMAIT/TEST1-16x2-RANDOM-DMAIT-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM/DMAIT/TEST1-16x2-RANDOM-DMAIT-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM/DMAIT/TEST1-16x2-RANDOM-DMAIT-SEED5-log.txt";


        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM2/BASELINE/TEST2-16x2-RANDOM-BASELINE-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM2/BASELINE/TEST2-16x2-RANDOM-BASELINE-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM2/BASELINE/TEST2-16x2-RANDOM-BASELINE-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM2/BASELINE/TEST2-16x2-RANDOM-BASELINE-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM2/BASELINE/TEST2-16x2-RANDOM-BASELINE-SEED5-log.txt";

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM2/DMAIT/TEST2-16x2-RANDOM-DMAIT-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM2/DMAIT/TEST2-16x2-RANDOM-DMAIT-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM2/DMAIT/TEST2-16x2-RANDOM-DMAIT-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM2/DMAIT/TEST2-16x2-RANDOM-DMAIT-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-RANDOM2/DMAIT/TEST2-16x2-RANDOM-DMAIT-SEED5-log.txt";

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED10-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED11-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED12-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED13-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED14-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/BASELINE/TEST1-16x2-GRID-SPARSE-BASELINE-SEED15-log.txt";


        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED10-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED11-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED12-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED13-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED14-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-SPARSE/DMAIT/TEST1-16x2-GRID-SPARSE-DMAIT-SEED15-log.txt";



        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED10-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED11-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED12-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED13-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED14-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/BASELINE/TEST1-16x2-GRID-DENSE-BASELINE-SEED15-log.txt";


        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/DMAIT/TEST1-16x2-GRID-DENSE-DMAIT-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/DMAIT/TEST1-16x2-GRID-DENSE-DMAIT-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/DMAIT/TEST1-16x2-GRID-DENSE-DMAIT-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/DMAIT/TEST1-16x2-GRID-DENSE-DMAIT-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/DMAIT/TEST1-16x2-GRID-DENSE-DMAIT-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/DMAIT/TEST1-16x2-GRID-DENSE-DMAIT-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/DMAIT/TEST1-16x2-GRID-DENSE-DMAIT-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/DMAIT/TEST1-16x2-GRID-DENSE-DMAIT-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/DMAIT/TEST1-16x2-GRID-DENSE-DMAIT-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/DMAIT/TEST1-16x2-GRID-DENSE-DMAIT-SEED10-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/DMAIT/TEST1-16x2-GRID-DENSE-DMAIT-SEED11-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/DMAIT/TEST1-16x2-GRID-DENSE-DMAIT-SEED12-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/DMAIT/TEST1-16x2-GRID-DENSE-DMAIT-SEED13-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/16x2-GRID-DENSE/DMAIT/TEST1-16x2-GRID-DENSE-DMAIT-SEED14-log.txt";


        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/TEST/TEST1-16x2-GRID-DENSE-TEST-DMAIT-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/TEST/TEST1-16x2-GRID-DENSE-TEST-DMAIT-SEED2-log.txt";

//COMPARISSONS

    //ALI,ALKHALEC
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-DMAIT-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-DMAIT-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-DMAIT-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-DMAIT-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-DMAIT-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-DMAIT-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-DMAIT-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-DMAIT-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-DMAIT-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-DMAIT-SEED10-log.txt";

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-BASELINE-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-BASELINE-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-BASELINE-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-BASELINE-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-BASELINE-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-BASELINE-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-BASELINE-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-BASELINE-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-BASELINE-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC/TEST1-12x1-JAMIL,ALI,ALKHALEC-BASELINE-SEED10-log.txt";

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-BASELINE-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-BASELINE-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-BASELINE-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-BASELINE-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-BASELINE-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-BASELINE-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-BASELINE-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-BASELINE-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-BASELINE-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-BASELINE-SEED10-log.txt";

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-DMAIT-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-DMAIT-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-DMAIT-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-DMAIT-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-DMAIT-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-DMAIT-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-DMAIT-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-DMAIT-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-DMAIT-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-JAMIL,ALI,ALKHALEC-OLD_NOATTACK/TEST1-12x1-ALI,ALKHALEC-DMAIT-SEED10-log.txt";

    //SHARMA,DHURANDHER

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/BASELINE/TEST1-16x2-SHARMA,DHURANDHER-BASELINE-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/BASELINE/TEST1-16x2-SHARMA,DHURANDHER-BASELINE-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/BASELINE/TEST1-16x2-SHARMA,DHURANDHER-BASELINE-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/BASELINE/TEST1-16x2-SHARMA,DHURANDHER-BASELINE-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/BASELINE/TEST1-16x2-SHARMA,DHURANDHER-BASELINE-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/BASELINE/TEST1-16x2-SHARMA,DHURANDHER-BASELINE-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/BASELINE/TEST1-16x2-SHARMA,DHURANDHER-BASELINE-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/BASELINE/TEST1-16x2-SHARMA,DHURANDHER-BASELINE-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/BASELINE/TEST1-16x2-SHARMA,DHURANDHER-BASELINE-SEED9-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/BASELINE/TEST1-16x2-SHARMA,DHURANDHER-BASELINE-SEED10-log.txt";

        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/DMAIT/TEST1-16x2-SHARMA,DHURANDHER-DMAIT-SEED1-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/DMAIT/TEST1-16x2-SHARMA,DHURANDHER-DMAIT-SEED2-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/DMAIT/TEST1-16x2-SHARMA,DHURANDHER-DMAIT-SEED3-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/DMAIT/TEST1-16x2-SHARMA,DHURANDHER-DMAIT-SEED4-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/DMAIT/TEST1-16x2-SHARMA,DHURANDHER-DMAIT-SEED5-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/DMAIT/TEST1-16x2-SHARMA,DHURANDHER-DMAIT-SEED6-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/DMAIT/TEST1-16x2-SHARMA,DHURANDHER-DMAIT-SEED7-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/DMAIT/TEST1-16x2-SHARMA,DHURANDHER-DMAIT-SEED8-log.txt";
        //String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/DMAIT/TEST1-16x2-SHARMA,DHURANDHER-DMAIT-SEED9-log.txt";
        String inputLogFile = "/home/daniel/contiki-ng/examples/EXPERIMENTS/COMPARISON-SHARMA,DHURANDHER/DMAIT/TEST1-16x2-SHARMA,DHURANDHER-DMAIT-SEED10-log.txt";

        String outputFile = inputLogFile.replace(".txt", "-PROCESSADO.txt");

        logFileNames = new ArrayList<String>();
        logFileNames.add(inputLogFile);
        //logFileNames.add("/home/daniel/contiki-ng/examples/5tree_dmait_log.txt");


        PrintStream out = new PrintStream(new FileOutputStream(outputFile, true), true);
        System.setOut(out);

        if (logFileNames != null) {
            LogParser.moteData = new MoteData();
            
            for (String logFileName : logFileNames) {
                File logFile = new File(logFileName);
                if (logFile != null && logFile.exists()) {            
                    System.out.println("Processando arquivo: " + logFileName);
                    FileParser fp = new FileParser(logFile, LogParser.moteData);
                    fp.processFile();

                } else {
                    System.out.println("Arquivo não existe: " + logFileName);
                }
            }

            //Print output
            HashMap<Integer, Mote> moteList = LogParser.moteData.getMoteList();
            Set<Integer> moteIds = moteList.keySet();
            for (Integer key : moteIds) {
                System.out.print(moteList.get(key).toString());
            }            
            
            
            CSVPrinter printer = new CSVPrinter(LogParser.moteData);                        
            System.out.print("\n\n");
            printer.printDelayCSV();
            System.out.print("\n\n");
            printer.printRTTDelayCSV();
            System.out.print("\n\n");
            printer.printLossCSV();
            System.out.print("\n\n");
            printer.printMoteParents();


            System.out.println("\n");
            System.out.println("\n");
            System.out.println("\n");
            System.out.println("\n");


        } else {
            printUsage();
        }

        System.out.flush();
        System.out.close();
    }
    
    private static void printUsage() {
        System.out.println("Forma de usar");
        System.out.println("java -jar LogParser.jar /home/user/log.txt /home/user/log2.txt /home/user/log3.txt [other logs]");
    }
    
    public static HashMap<String, String> tokenizeArgs(String[] args) {
        HashMap<String, String> result = new HashMap<String, String>();
        
        for(String arg : args) {
            int delimiterIndex = arg.indexOf(LogParser.ARGUMENT_VALUE_DELIMITER);
            if (delimiterIndex >= 0) {
                String key = arg.substring(0, delimiterIndex).trim();
                String value = arg.substring(delimiterIndex).replaceAll(LogParser.ARGUMENT_VALUE_DELIMITER, "").trim();
                result.put(key, value);
            } else {
                result.put(arg, null);
            }
        }
        
        return result;
    }
}
