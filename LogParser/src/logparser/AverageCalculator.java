/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package logparser;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.ArrayList;

/**
 *
 * @author daniel
 */
public class AverageCalculator {
    private ArrayList<BigDecimal> values = new ArrayList<BigDecimal>();

    
    public void addValue(Long value) {
        this.values.add(new BigDecimal(value));        
    }
    
    public void addValue(BigDecimal value) {
        this.values.add(value);        
    }    
    
    public BigDecimal calculateAverage() {
        BigDecimal total = null;
        if (!this.values.isEmpty()) {       
            total = new BigDecimal(0);
            for (BigDecimal value : this.values) {
                total = total.add(value);
            }            
            total = total.divide(new BigDecimal(this.values.size()), 2, RoundingMode.HALF_UP);
        }
        return total;
    }
 
}
