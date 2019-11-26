// Edited By Prikshet Sharma, 2019.A
//
// Simple Java implementation of the classic Dining Philosophers problem.
//
// No synchronization (yet).
//
// Graphics are *very* naive.  Philosophers are big blobs.
// Forks are little blobs.
// 
// Written by Michael Scott, 1997; updated 2013 to use Swing.
// Updated again in 2019 to drop support for applets.
 

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import java.util.*;
import java.lang.*;
import java.lang.Thread.*;
import java.util.HashMap;
import javax.imageio.ImageIO;
// This code has six main classes:
//  Dining
//      The public, "main" class.
//  Philosopher
//      Active -- extends Thread
//  Fork
//      Passive
//  Table
//      Manages the philosophers and forks and their physical layout.
//  Coordinator
//      Provides mechanisms to suspend, resume, and reset the state of
//      worker threads (philosophers).
//  UI
//      Manages graphical layout and button presses.

public class Dining {
    private static final int CANVAS_SIZE = 360;
    // pixels in each direction;
    // needs to agree with size in dining.html

    public int getCanvasSize() {
	return CANVAS_SIZE;
    }
    public static void main(String[] args) {
        JFrame f = new JFrame("Dining");
        f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        Dining me = new Dining();

        final Coordinator c = new Coordinator();
        final Table t = new Table(c, CANVAS_SIZE);
        // arrange to call graphical setup from GUI thread
        try {
            SwingUtilities.invokeAndWait(new Runnable() {
		    public void run() {
			new UI(f, c, t);
		    }
		});
        } catch (Exception e) {
            System.err.println("unable to create GUI");
        }

        f.pack();            // calculate size of frame
        f.setVisible(true);
    }
}

class Fork extends Item {

    private boolean isDirty;
    public Fork(Table T, int cx, int cy) {
	super(T, cx, cy);
	//System.err.println("Placing " + this.getName() + " at " + cx + " " + cy);
	isDirty = true;
    }
    

    public void setClean() {
	isDirty = false;
    }

    public void setDirty() {
	isDirty = true;
    }

    public boolean isDirty() {
	return isDirty;
    }

    public String getName() {
	return "Fork " + (this + "").substring(5, 8);
    }

    public void reset() {
	super.reset();	
    }
    public void draw(Graphics g) {
	g.setColor(Color.gray);
	super.draw(this.getName(), g);
    }
}
class Item {
    private Table t;
    private static final int XSIZE = 10;
    private static final int YSIZE = 10;
    private final int orig_x;
    private final int orig_y;
    private int x;
    private int y;

    // Constructor.
    // cx and cy indicate coordinates of center.
    // Note that fillOval method expects coordinates of upper left corner
    // of bounding box instead.
    public Item(Table T, int cx, int cy) {
	t = T;
	orig_x = cx;
	orig_y = cy;
	x = cx;
        y = cy;
    }

    enum Status {
	SENT, RECEIVED;
    }

    Status status;
    Status tokenStatus;
    Philosopher sender;
    public void send(Philosopher p) {
	status = Status.SENT;
	sender = p;
    }

    public Philosopher getSender() {
	return sender;
    }

    public boolean isSent() {
	return status == Status.SENT;
    }

    public boolean isRequestSent() {
	return tokenStatus == Status.SENT;
    }

    public void sendRequest() {
	tokenStatus = Status.SENT;
    }

    public void receive() {
	status = Status.RECEIVED;
    }

    public void receiveRequest() {
	tokenStatus = Status.RECEIVED;
    }

    // erase self
    private void clear() {
        Graphics g = t.getGraphics();
	g.setColor(t.getBackground());	
        g.fillOval(x-XSIZE/2, y-YSIZE/2, XSIZE, YSIZE);
    }

    public void reset() {
	clear();
	x = orig_x;
	y = orig_y;
	t.repaint();
        
    }

    // arguments are coordinates of acquiring philosopher's center
    //
    public void acquire(int px, int py, int numForks) {
	if (t.getGraphics() != null)
	    clear();
        x = (orig_x + px) / 2 + numForks * 10;
        y = (orig_y + py) / 2 + numForks * 10;
	///System.err.println("acquired");
	t.repaint();
    }
    
    // render self
    public void draw(String name, Graphics g) {
	//g.drawString(name.contains("Bottle") ? name.substring(6) : name.substring(4), x, y);
	try {
	    //prepare a original Image source
	    Image image = ImageIO.read(t.getClass().getResource("fork.png"));
	    
	    int w = image.getWidth(null)/3;
	    int h = image.getHeight(null)/3;
	     
	    g.drawImage(image, x, y, w, h, null);
	    
            
	} catch (IOException ex) {
	    System.err.println("error loading image");
	}
	//g.drawString(i + "", x, y);
        //g.fillOval(x-XSIZE/2, y-YSIZE/2, XSIZE, YSIZE);
	
    }
   
}

class Bottle extends Item {
    private Fork correspondingFork;
    
    public Bottle(Table T, int cx, int cy) {
	super(T, cx, cy);
    }
    
    public void setCorrespondingFork(Fork fork) {
	correspondingFork = fork;
    }

    public Fork getCorrespondingFork() {
	return correspondingFork;
    }

    public String getName() {
	return "Bottle " + (this + "").substring(8, 11);
    }

    public void reset() {
	System.err.print(this.getName());
	super.reset();	
    }

    public void draw(Graphics g) {
	g.setColor(Color.pink);
	super.draw(this.getName(), g);
    }
}



class Drinker extends Thread {
    private static final Color DRINK_COLOR = Color.green;
    private static final Color TRANQUIL_COLOR = Color.blue;
    private static final Color WAIT_COLOR = Color.red;
    
    private static final double TRANQUIL_TIME = 4.0;
    private static final double FUMBLE_TIME = 2.0;
    private static final double DRINK_TIME = 3.0;
    private Map<Bottle, Boolean> bottles = new HashMap<Bottle, Boolean>();    
    private enum Status {
	TRANQUIL, DRINKS, THIRSTY;
    }

    private Status status;
    private Map<Bottle,Boolean> hasRequestToken = new HashMap<Bottle,Boolean>();

    private Map<Bottle, Boolean> needsBottle = new HashMap<Bottle, Boolean>();

    private Coordinator c;
    private Table t;
    private static final int XSIZE = 50;
    private static final int YSIZE = 50;
    private int x;
    private int y;

    Philosopher diner;
    private Random prn;
    private Color color;

    public int getNumBottles() {
	int numBottles = 0;
	for (boolean hasBottle : bottles.values()) {
	    if (hasBottle) numBottles++;
	}
	return numBottles;
    }
    public int getX() {
	return diner.getX();
    }

    public int getY() {
	return diner.getY();
    }

    
    public void setBottle(Bottle bottle, boolean bool) {
	bottles.put(bottle, bool);
    }

    public Map<Bottle, Boolean> getBottles() {
	return bottles;
    }
    
    public boolean getRequestToken(Bottle bottle) {
	return hasRequestToken.get(bottle);
    }

    public void setRequestToken(Bottle bottle, boolean bool) {
	hasRequestToken.put(bottle, bool);
    }
    
    
    public Philosopher getPhilosopher() {
	return diner;
    }
    public boolean isThirsty() {
	return status == Status.THIRSTY;
    }
    
    
    public String getPhilosopherName() {
	return diner.getPhilosopherName();
    }
    

    public void addBottle(Bottle bottle, Boolean bool) {
	bottles.put(bottle, bool);
    }


    	
    // Constructor.
    // cx and cy indicate coordinates of center
    // Note that fillOval method expects coordinates of upper left corner
    // of bounding box instead.
    //
    public Drinker(Table T, int cx, int cy,
		   Coordinator C, String name, Philosopher p) {
	t = T;
        x = cx;
        y = cy;
	diner = p;
	bottles = new HashMap<Bottle, Boolean>();
        c = C;
        prn = new Random();
        color = TRANQUIL_COLOR;
	
	status = Status.TRANQUIL;
	
    }

    
    
    // render self
    public void draw(Graphics g, int i) {
        g.setColor(color);
        //g.fillOval(x-XSIZE/2, y-YSIZE/2, XSIZE, YSIZE);
	g.drawString(i + "", x, y);
    }

    // sleep for secs +- FUDGE (%) seconds
    //
    private static final double FUDGE = 0.2;
    private void delay(double secs) throws ResetException {
        double ms = 1000 * secs;
        int window = (int) (2.0 * ms * FUDGE);
        int add_in = prn.nextInt() % window;
        int original_duration = (int) ((1.0-FUDGE) * ms + add_in);
        int duration = original_duration;
        for (;;) {
            try {
                Thread.sleep(duration);
                return;
            } catch(InterruptedException e) {
                if (c.isReset()) {
                    throw new ResetException();
                } else {        // suspended
                    c.gate();   // wait until resumed
                    duration = original_duration / 2;
                    // don't wake up instantly; sleep for about half
                    // as long as originally instructed
                }
            }
        }
    }
    
    
    private void acquireStartingBottles() {
	int numAcquired = 0;
	for (Bottle bottle : bottles.keySet().toArray(new Bottle[0])) {
	    if (bottles.get(bottle)) {
		//System.err.println(bottle.getName() + "acquired");
		bottle.acquire(x, y, numAcquired);
		numAcquired++;
	    }
	}
    }
    // start method of Thread calls run; you don't
    //

    public void run() {
	DrinkerReceive receiveListener = new DrinkerReceive(this);
	receiveListener.start();
	acquireStartingBottles();
	for (;;) {
            try {
                if (c.gate()) {
		    delay(DRINK_TIME/2.0);
		}
		
                tranquil();
        
                if (c.gate()) {
		    delay(TRANQUIL_TIME/2.0);
		}
		
                thirst();

		if (c.gate()) {
		    delay(FUMBLE_TIME/2.0);
		}
		
                drink();

	    } catch(ResetException e) { 
                color = TRANQUIL_COLOR;
                t.repaint();
            }
        }
    }
    // Drinking Actions
    private void sendBottle(Bottle bottle) {
	if (hasRequestToken.get(bottle) && bottles.get(bottle) && !(needsBottle.get(bottle) && (status == Status.DRINKS || diner.getForks().get(bottle.getCorrespondingFork())))) {
	    bottle.send(this.diner);
	    bottles.put(bottle, false);
	} else {
	    //System.err.println(getPhilosopherName() + " doesn't send bottle " + bottle.getName());
	}
    }


    private void sendBottles() {
        for (Bottle bottle : bottles.keySet().toArray(new Bottle[0])) {
	    if (bottles.get(bottle)) {
		    sendBottle(bottle);
		    yield();
		}
	}
	
	//System.err.println(getPhilosopherName() + " has released all forks");
    }
    private void setNoBottlesNeeded() {
	for (Bottle bottle : bottles.keySet().toArray(new Bottle[0])) {
	    needsBottle.put(bottle, false);
	}
	// System.err.println(getPhilosopherName() + " needs no bottles");
    }
    
    private void tranquil() throws ResetException {
	status = Status.TRANQUIL;
	setNoBottlesNeeded();
	System.out.println(getPhilosopherName() + " is tranquil");
	color = TRANQUIL_COLOR;
	t.repaint();
        delay(TRANQUIL_TIME);
    }

    private void needRandomSetOfBottles() {
	Random random = new Random();
	int numBottlesNeeded = random.nextInt(bottles.keySet().size() - 1) + 1;
	Bottle[] bottleId = bottles.keySet().toArray(new Bottle[0]);
	ArrayList<Bottle> bottlesClaimed = new ArrayList<Bottle>();
	int bottlesSampled = 0;
	while (bottlesSampled < numBottlesNeeded) {
	    int randomBottle = random.nextInt(numBottlesNeeded);
	    if (!bottlesClaimed.contains(bottleId[randomBottle])) {
		bottlesClaimed.add(bottleId[randomBottle]);
		needsBottle.put(bottleId[randomBottle], true);
		bottlesSampled++;
	    }
	}
	assert(bottlesClaimed.size() == numBottlesNeeded);

	//System.err.println(getPhilosopherName() + " needs " + numBottlesNeeded + " bottles");
	
	// for (Bottle b : bottlesClaimed) {
	//     System.err.println("    " + getPhilosopherName() + " has " + b.getName() +  "? " + bottles.get(b));
	// }
    }

    private boolean hasAllBottles() {
	for (boolean isReceived : bottles.values()) {
	    if (!isReceived) {
		return false;
	    }
	}
	return true;
    }

    private void requestBottle(Bottle bottle) {
	if (status == Status.THIRSTY && needsBottle.get(bottle) && hasRequestToken.get(bottle) && !bottles.get(bottle)) {
	    bottle.sendRequest();
	    //System.err.println(getPhilosopherName() + " requests " + bottle.getName());
	    hasRequestToken.put(bottle, false);
	} else {
	    // System.err.println(getPhilosopherName() + " doesn't request bottle " + bottle.getName() + " because");
	    // System.err.println(getPhilosopherName() + " is thirsty? " + (status == Status.THIRSTY));
	    // System.err.println(getPhilosopherName() + " needs bottle? " + needsBottle.get(bottle));
	    // System.err.println(getPhilosopherName() + " has request token " + bottle.getName() + "? " + hasRequestToken.get(bottle));
	    // System.err.println(getPhilosopherName() + " has bottle? " + bottles.get(bottle));
	}
    }

    private void requestBottles() {
	for (Bottle bottle : bottles.keySet().toArray(new Bottle[0])) {

	    if (needsBottle.get(bottle)) {
		requestBottle(bottle);
	    }

	    yield();
	}
	//System.err.println(getPhilosopherName() + " has requested all bottles");
    }
    
    private void thirst() throws ResetException {
	status = Status.THIRSTY;
	color = WAIT_COLOR;
        t.repaint();
	System.out.println(getPhilosopherName() + " is thirsty");
        delay(FUMBLE_TIME);
	needRandomSetOfBottles();
	requestBottles();
	// System.err.println("in thirst: is " + diner.getPhilosopherName() + " hungry? " + diner.getHungry());
	// System.err.println("WAIT for " + getPhilosopherName() + " to have all bottles");;
	while (!hasAllBottles()) {} // be hungry until all forks received
	//System.err.println("WAIT ENDS " + getPhilosopherName() + " has all bottles");
    }

    private void drink() throws ResetException {
	status = Status.DRINKS;
	System.out.println(getPhilosopherName() + " drinks");
	color = DRINK_COLOR;
	t.repaint();
        delay(DRINK_TIME);
    }

    public void addRequestToken(Bottle bottle, boolean bool) {
	hasRequestToken.put(bottle, bool);
    }

    public Map<Bottle, Boolean> getRequestTokens() {
	return hasRequestToken;
    }

}


class DrinkerReceive extends Thread {
    Drinker drinker;

    public DrinkerReceive(Drinker d) {
	drinker = d;
    }

        private boolean isRequestSentToMe(Bottle bottle) {
	return bottle.isRequestSent() && !drinker.getRequestToken(bottle);
    }

    // if bottle's status is sent and this Philosopher doesn't have the bottle,
    // then it must have been sent by the other Philosopher. 
    private boolean isSentToMe(Bottle bottle) {
	return bottle.isSent() && bottle.getSender() != drinker.getPhilosopher();
    }
    
    private void receiveRequest(Bottle bottle) {
	if (isRequestSentToMe(bottle)) {
	    drinker.setRequestToken(bottle, true);
	    bottle.receiveRequest();
	} else {
	    // System.err.println("request not received for " + bottle.getName() + " because");
	    // System.err.println("    is request sent for " + bottle.getName() + "? " + bottle.isRequestSent());
	    // System.err.println("    " + getPhilosopherName() + " has request token? " + hasRequestToken.get(bottle));
	}
    }

    private void receiveBottle(Bottle bottle) {
	if (isSentToMe(bottle)) {
	    drinker.setBottle(bottle, true);
	    //System.err.println(drinker.getPhilosopherName() + " receives bottle " + bottle);
	    bottle.receive();
	    bottle.acquire(drinker.getX(), drinker.getY(), drinker.getNumBottles());
	}
    }

    private void receiveBottles() {
	for (Bottle bottle : drinker.getBottles().keySet().toArray(new Bottle[0])) {
	    receiveBottle(bottle);
	    yield();
	}	
    }

    private void receiveRequests() {
	for (Bottle bottle : drinker.getBottles().keySet().toArray(new Bottle[0])) {
	    receiveRequest(bottle);
	    yield();
	}	
    }

    public void run() {
	for(;;) {
	    receiveRequests();
	    receiveBottles();
	}
    }
    
}

class DinerReceive extends Thread {
    Philosopher diner;
    public DinerReceive(Philosopher p) {
	diner = p;
    }

    
    private void receiveFork(Fork f) {
	if (isSentToMe(f)) {
	    //System.err.println(diner.getPhilosopherName() + " receives fork " + f.getName());
	    f.receive();
	    f.setClean();
	    f.acquire(diner.getX(), diner.getY(), diner.getNumForks());
	    diner.setFork(f, true);
	}

	// debug
	else {
	    // if (!f.isSent()) {
	    // 	System.err.println(f.getName() + " hasn't been sent to " + getPhilosopherName() + " by other philosopher");
	    // }
	    // if (forks.get(f)) {
	    // 	System.err.println(getPhilosopherName() + " already has " + f.getName());
	    // }
	}
    }
    
    private void receiveForks() {
	for (Fork f : diner.getForks().keySet().toArray(new Fork[0])) {
	    receiveFork(f);
	    yield();
	}
    }

        private void receiveRequest(Fork f) {
	if (isRequestSentToMe(f)) {
	    f.receiveRequest();
	    diner.setRequestToken(f, true);
	} else {
	    // System.err.println("request not received for " + f.getName() + " because");
	    // System.err.println("    is request sent for " + f.getName() + "? " + f.isRequestSent());
	    // System.err.println("    " + getPhilosopherName() + " has request token? " + hasRequestToken.get(f));
	}
    }

    private boolean isRequestSentToMe(Fork f) {
	return f.isRequestSent() && !diner.getRequestToken(f);
    }
    // if fork's status is sent and this philosopher doesn't have
    // the fork then it must have been sent by the other philosopher
    private boolean isSentToMe(Fork f) {
	return f.isSent() && f.getSender() != diner;
    }

    private void receiveRequests() {
	for (Fork f : diner.getForks().keySet().toArray(new Fork[0])) {
	    receiveRequest(f);
	    yield();
	}	
    }

    public void run() {
	for (;;) {
	    receiveForks();
	    receiveRequests();
	}
    }
}

class Philosopher extends Thread {

    private Coordinator c;
    private Table t;
    private static final int XSIZE = 50;
    private static final int YSIZE = 50;
    private int x;
    private int y;

    private static final Color THINK_COLOR = Color.blue;
    private static final Color WAIT_COLOR = Color.red;
    private static final Color EAT_COLOR = Color.green;

    private Color color;

    private static final double THINK_TIME = 4.0;
    
    private static final double FUMBLE_TIME = 2.0;
    // time between becoming hungry and grabbing first fork
    private static final double EAT_TIME = 3.0;
    private Map<Fork,Boolean> forks = new HashMap<Fork,Boolean>();

    private Random prn;

    
    private Drinker drinker;


    // public void drawDrinker(Graphics g, int i) {
    // 	drinker.draw(g, i);
    // }

    
    public boolean getRequestToken(Fork f) {
	return hasRequestToken.get(f);
    }
    public void setRequestToken(Fork f, boolean b) {
	hasRequestToken.put(f, b);
    }
    
    public int getX() {
	return x;
    }

    public int getY() {
	return y;
    }
    public void setFork(Fork f, boolean b) {
	forks.put(f, b);
    }
    public boolean getFork(Fork f) {
	return forks.get(f);
    }
    
    public int getNumForks() {
	int numForks = 0;
	for (boolean hasFork : forks.values()) {
	    if (hasFork) numForks++;
	}
	return numForks;
    }
    
    public void addDrinkerRequestToken(Bottle bottle, Boolean bool) {
	drinker.addRequestToken(bottle, bool);
    }

    public Map<Bottle, Boolean> getDrinkerRequestTokens() {
	return drinker.getRequestTokens();
    }

    public void addBottle(Bottle bottle, Boolean bool) {
	drinker.addBottle(bottle, bool);
    }

    public Map<Fork, Boolean> getForks() {
	return forks;
    }

    public boolean getHungry() {
	return status == Status.HUNGRY;
    }
    
    private enum Status {
	THINKS, HUNGRY, EATS;
    }
    private Status status;

    private Map<Fork, Boolean> hasRequestToken = new HashMap<Fork, Boolean>();

    private void acquireStartingForks() {
	int numAcquired = 0;
	for (Fork f : forks.keySet().toArray(new Fork[0])) {
	    if (forks.get(f)) {
		//System.err.println(f.getName() + "acquired");
		f.acquire(x, y, numAcquired);
		numAcquired++;
	    }
	}
    }
    // Thread start method calls run
    public void run() {
	acquireStartingForks();
	DinerReceive receiveListener = new DinerReceive(this);
	receiveListener.start();
	//drinker.start();
	for (;;) {
            try {
                if (c.gate()) {
		    delay(EAT_TIME/2.0);
		}
		
                think();
        
                if (c.gate()) {
		    delay(THINK_TIME/2.0);
		}
		
                hunger();

		if (c.gate()) {
		    delay(FUMBLE_TIME/2.0);
		}
		
                eat();

	    } catch(ResetException e) { 
                color = THINK_COLOR;
                t.repaint();
            }
        }
    }
    
    // Dining Actions
    
    private void releaseFork(Fork f) {
	if (status != Status.EATS && hasRequestToken.get(f) && f.isDirty()) {
	    //System.err.println(getPhilosopherName() + " sends fork " + f.getName());
	    f.send(this);
	    f.setClean();
	    forks.put(f, false);
	    f.reset();
	} else {
	    
	    // System.err.println(getPhilosopherName() + " doesn't send " + f.getName());
	    // if (status == Status.EATS) System.err.println(getPhilosopherName() + " is eating");
	    // if (!hasRequestToken.get(f)) System.err.println(getPhilosopherName() + " doesn't have request token");
	    // if (!f.isDirty()) System.err.println(f.getName() + " is clean");
	    
	}
    }


    private void think() throws ResetException {
	status = Status.THINKS;
	releaseMyForks();
	System.out.println(getPhilosopherName() + " thinking");
	color = THINK_COLOR;
	t.repaint();
        delay(THINK_TIME);
	//drinker's check
	//System.err.println("WAIT for " + getPhilosopherName() + " to be thirsty");
	//while (!drinker.isThirsty()) {}
	//System.err.println("WAIT ENDS " + getPhilosopherName() + " is thirsty");
    }
    private void requestFork(Fork f) {
	if (status == Status.HUNGRY && hasRequestToken.get(f) && !forks.get(f)) {
	    
	    //System.out.println(getPhilosopherName() + " requests " + f.getName());
	    f.sendRequest();
	    hasRequestToken.put(f, false);
	} else {
	    if (!hasRequestToken.get(f)) {
		//System.err.println(getPhilosopherName() + " doesn't have request token");
	    }
	    if (forks.get(f)) {
		//System.err.println(getPhilosopherName() + " already has " + f.getName());
	    }
	    
	    
	}
    }

    private void requestForks() {
	for (Fork f : forks.keySet().toArray(new Fork[0])) {
	    requestFork(f);
	    yield();
	}
	//System.err.println(getPhilosopherName() + " has requested all forks");
    }

    
    private boolean hasAllForks() {
	for (boolean isReceived : forks.values()) {
	    if (!isReceived) {
		return false;
	    }
	}
	//System.err.println(getPhilosopherName() + " has all forks");
	return true;
    }
    private void hunger() throws ResetException {
	status = Status.HUNGRY;
	
	releaseMyForks();
	color = WAIT_COLOR;
        t.repaint();
	System.out.println(getPhilosopherName() + " waiting");
        delay(FUMBLE_TIME);
	requestForks();

	
	// System.err.println("WAIT for " + getPhilosopherName() + " to have all forks");
	while (!hasAllForks()) {} // be hungry until all forks received
	// System.err.println("WAIT ENDS " + getPhilosopherName() + " has all forks");
    }

    private void dirtyMyForks() {
	for (Fork fork : forks.keySet().toArray(new Fork[0])) {
	    fork.setDirty();
	}
    }

    private void releaseMyForks() {
        for (Fork fork : forks.keySet().toArray(new Fork[0])) {
	    if (forks.get(fork)) {
		    releaseFork(fork);
		    yield();
		}
	}
	
	//System.err.println(getPhilosopherName() + " has released all forks");
    }

    private void eat() throws ResetException {
	status = Status.EATS;
	System.out.println(getPhilosopherName() + " eating");
	color = EAT_COLOR;
	t.repaint();
	dirtyMyForks();
        delay(EAT_TIME);
	
	// drinker's extension
	// System.err.println("WAIT for " + getPhilosopherName() + " to not be thirsty");
	//while (drinker.isThirsty()) {}
	// System.err.println("WAIT ENDS " + getPhilosopherName() + " isn't thirsty");
    }


    public Philosopher(Table T, int cx, int cy, Coordinator C) {
	t = T;
        x = cx;
        y = cy;
	
	drinker = new Drinker(T, cx, cy, C, getPhilosopherName(), this);
        
	forks = new HashMap<Fork, Boolean>();
        c = C;
        prn = new Random();
        color = THINK_COLOR;

	status = Status.THINKS;
    }
    
    public void printForks() {
	//System.err.println(getPhilosopherName() + "'s Forks");
	for (Fork f : forks.keySet().toArray(new Fork[0])) {
	    //  System.err.println(getPhilosopherName() + " has " + f.getName() + "? " + forks.get(f));
	}
    }

    public void addFork(Fork f, Boolean b) {
	forks.put(f, b);
    }

    public void addRequestToken(Fork f, boolean bool) {
	hasRequestToken.put(f, bool);
    }

    public Map<Fork, Boolean> getRequestTokens() {
	return hasRequestToken;
    }

    // sleep for secs +- FUDGE (%) seconds
    //
    private static final double FUDGE = 0.2;
    private void delay(double secs) throws ResetException {
        double ms = 1000 * secs;
        int window = (int) (2.0 * ms * FUDGE);
        int add_in = prn.nextInt() % window;
        int original_duration = (int) ((1.0-FUDGE) * ms + add_in);
        int duration = original_duration;
        for (;;) {
            try {
                Thread.sleep(duration);
                return;
            } catch(InterruptedException e) {
                if (c.isReset()) {
                    throw new ResetException();
                } else {        // suspended
                    c.gate();   // wait until resumed
                    duration = original_duration / 2;
                    // don't wake up instantly; sleep for about half
                    // as long as originally instructed
                }
            }
        }
    }

    // render self
    public void draw(Graphics g, int i) {
	
        g.setColor(color);
        //g.fillOval(x-XSIZE/2, y-YSIZE/2, XSIZE, YSIZE);
	String fileName;
	if (i == 4) {
	    fileName = "5.png";
	} else {
	    Integer fileNum = i + 1;
	    fileName = fileNum.toString() + ".jpg";
	}
	//System.out.println("fileName:" + fileName);

	try {
	    //prepare a original Image source
	    Image image = ImageIO.read(t.getClass().getResource(fileName));
	    
	    int w = image.getWidth(null)/3;
	    int h = image.getHeight(null)/3;

	    if (fileName.equals("5.png")) w = w * 7/10;
	    if (fileName.equals("5.png")) h = h * 7/10;
	    //g.drawImage(image, 0, 0, w, h, null);
            if (fileName.equals("1.jpg")) {
		y -= 20;
		x += 10;
	    } 
	    g.drawImage(image, x, y, w, h, null);
	    if (fileName.equals("1.jpg")) {
		y += 20;
		x -= 10;
	    }
            
	} catch (IOException ex) {
	    System.err.println("error loading image");
	}
	g.drawString(i + "", x, y);
    }
    
    public String getPhilosopherName() {
    	return "Philosopher " + Integer.parseInt((this + "").substring(14, 15)) / 2;
    }

}


// Graphics panel in which philosophers and forks appear.
//
class Table extends JPanel {
    private static int NUM_PHILS = 5;
    private static int numForks = 5;
    private static int numBottles = 5;
    // following fields are set by construcctor:
    private final Coordinator c;
    private Fork[] forks;
    private Philosopher[] philosophers;
    private Bottle[] bottles;

    public void pause() {
        c.pause();
        // force philosophers to notice change in coordinator state:
        for (int i = 0; i < NUM_PHILS; i++) {
            philosophers[i].interrupt();
        }
    }

    // Called by the UI when it wants to start over.
    //
    public void reset() {
        c.reset();
        // force philosophers to notice change in coordinator state:
        for (int i = 0; i < NUM_PHILS; i++) {
            philosophers[i].interrupt();
        }
        for (int i = 0; i < numForks; i++) {
            forks[i].reset();
        }

	for (int i = 0; i < numBottles; i++) {
            bottles[i].reset();
        }
    }

    // The following method is called automatically by the graphics
    // system when it thinks the Table canvas needs to be re-displayed.
    // This can happen because code elsewhere in this program called
    // repaint(), or because of hiding/revealing or open/close
    // operations in the surrounding window system.
    //
    public void paintComponent(Graphics g) {
	if (forks != null && philosophers != null) {
	    
	    super.paintComponent(g);
        
	    for (int i = 0; i < numForks; i++) {
		forks[i].draw(g);
		//bottles[i].draw(g);
	    }
	    for (int i = 0; i < NUM_PHILS; i++) {
		
		philosophers[i].draw(g, i);
	    }
	    
	}
	g.setColor(Color.black);
	g.drawRect(0, 0, getWidth()-1, getHeight()-1);
    }


    private boolean ifStarred(String string) {
	return string.length() == 2 && string.charAt(1) == '*';
    }
    // Constructor
    //
    // Note that angles are measured in radians, not degrees.
    // The origin is the upper left corner of the frame.
    //
    public Table(Coordinator C, int CANVAS_SIZE) {    // constructor

	
	
	c = C;
        setPreferredSize(new Dimension(CANVAS_SIZE, CANVAS_SIZE));
	
	try {
	    
	    BufferedReader in = new BufferedReader(new FileReader("graph.txt"));
	    
	    Object[] lines = in.lines().toArray();
	    NUM_PHILS = Integer.parseInt(lines[0].toString().split(" ")[0]);
	    numForks = Integer.parseInt(lines[1].toString().split(" ")[0]);
	    numBottles = numForks;
	    forks = new Fork[numForks];
	    bottles = new Bottle[numBottles];
	    philosophers = new Philosopher[NUM_PHILS];
	    
	    for (int i = 0; i < numForks; i++) {
		double angle = Math.PI/2 + 2*Math.PI/NUM_PHILS*(i-0.5);
		// place forks and bottles according to graph.txt
		forks[i] = new Fork(this,
				    (int) (CANVAS_SIZE/2.0),
				    (int) (CANVAS_SIZE/2.0));
		bottles[i] = new Bottle(this,
					(int) (CANVAS_SIZE/2.0),
					(int) (CANVAS_SIZE/2.0));
		 
	    }
	    
	    for (int i = 0; i < NUM_PHILS; i++) {
		double angle = Math.PI/2 + 2*Math.PI/NUM_PHILS*i;
	    
		philosophers[i] = new Philosopher(this,
						  (int) (CANVAS_SIZE/2.0 + CANVAS_SIZE/3.0 * Math.cos(angle)),
						  (int) (CANVAS_SIZE/2.0 - CANVAS_SIZE/3.0 * Math.sin(angle)),
						  c);            
	    }

	
	    int philosopherCount = 0;
	    int forkAndBottleCount = 0;
	    for (Object l : Arrays.copyOfRange(lines, 2, lines.length)) {
		if (!l.toString().equals("-") && !l.toString().equals("\n") && l.toString().length() != 0) {
		    for (String neighborString : l.toString().split(" ")) {
			int neighborInt = Integer.parseInt(neighborString.substring(0, 1));

			philosophers[philosopherCount].addFork(forks[forkAndBottleCount], true);
			philosophers[philosopherCount].addRequestToken(forks[forkAndBottleCount], false);
			philosophers[philosopherCount].addBottle(bottles[forkAndBottleCount], true);
			philosophers[philosopherCount].addDrinkerRequestToken(bottles[forkAndBottleCount], false);
			philosophers[neighborInt].addFork(forks[forkAndBottleCount], false);
			philosophers[neighborInt].addRequestToken(forks[forkAndBottleCount], true);
			philosophers[neighborInt].addBottle(bottles[forkAndBottleCount], false);
			philosophers[neighborInt].addDrinkerRequestToken(bottles[forkAndBottleCount], true);
			forkAndBottleCount++;
		    }
		}
		philosopherCount++;
	    }

	    
	    // for (int i = 0; i < NUM_PHILS; i++) {
		
	    // 	System.err.println(philosophers[i].getPhilosopherName() + "'s Eat tokens:");
	    // 	for (Fork fork : philosophers[i].getRequestTokens().keySet().toArray(new Fork[0])) {
	    // 	    System.err.println(fork.getName() + " : " + philosophers[i].getRequestTokens().get(fork));
	    // 	}    
	    // }

	    // for (int i = 0; i < NUM_PHILS; i++) {
		
	    // 	System.err.println(philosophers[i].getPhilosopherName() + "'s forks:");
	    // 	for (Fork fork : philosophers[i].getForks().keySet().toArray(new Fork[0])) {
	    // 	    System.err.println(fork.getName() + " : " + philosophers[i].getForks().get(fork));
	    // 	}    
	    // }
	    
	    // // for (int i = 0; i < NUM_PHILS; i++) {
	    // // 	System.err.println(philosophers[i].getPhilosopherName() + "'s Drink tokens:");
	    // // 	for (Bottle bottle : philosophers[i].getDrinkerRequestTokens().keySet().toArray(new Bottle[0])) {
	    // // 	    System.err.println(bottle.getName() + " : " + philosophers[i].getDrinkerRequestTokens().get(bottle));
	    // // 	}
	    // // }
	    // System.err.println("\n\nStart");
	    for (int i = 0; i < NUM_PHILS; i++) {
		philosophers[i].start();
	    }
	} catch (Exception e) {
	    System.err.println(e);
	    System.err.println("Couldn't get neighbors");
	    
	}
        
    }
}

class ResetException extends Exception { };

// The Coordinator serves to slow down execution, so that behavior is
// visible on the screen, and to notify all running threads when the user
// wants them to reset.
//
class Coordinator {
    public enum State { PAUSED, RUNNING, RESET }
    private State state = State.PAUSED;

    public synchronized boolean isPaused() {
        return (state == State.PAUSED);
    }

    public synchronized void pause() {
        state = State.PAUSED;
    }

    public synchronized boolean isReset() {
        return (state == State.RESET);
    }

    public synchronized void reset() {
        state = State.RESET;
    }

    public synchronized void resume() {
        state = State.RUNNING;
        notifyAll();        // wake up all waiting threads
    }

    // Return true if we were forced to wait because the coordinator was
    // paused or reset.
    //
    public synchronized boolean gate() throws ResetException {
        if (state == State.PAUSED || state == State.RESET) {
            try {
                wait();
            } catch(InterruptedException e) {
                if (isReset()) {
                    throw new ResetException();
                }
            }
            return true;        // waited
        }
        return false;           // didn't wait
    }
}

// Class UI is the user interface.  It displays a Table canvas above
// a row of buttons.  Actions (event handlers) are defined for each of
// the buttons.  Depending on the state of the UI, either the "run" or
// the "pause" button is the default (highlighted in most window
// systems); it will often self-push if you hit carriage return.
//
class UI extends JPanel {
    private final Coordinator c;
    private final Table t;

    private final JRootPane root;
    private static final int externalBorder = 6;

    private static final int stopped = 0;
    private static final int running = 1;
    private static final int paused = 2;

    private int state = stopped;

    // Constructor
    //
    public UI(RootPaneContainer pane, Coordinator C, Table T) {
        final UI u = this;
        c = C;
        t = T;

        final JPanel b = new JPanel();   // button panel

        final JButton runButton = new JButton("Run");
        final JButton pauseButton = new JButton("Pause");
        final JButton resetButton = new JButton("Reset");
        final JButton quitButton = new JButton("Quit");

        runButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    c.resume();
		    root.setDefaultButton(pauseButton);
		}
	    });
        pauseButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    t.pause();
		    root.setDefaultButton(runButton);
		}
	    });
        resetButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    t.reset();
		    root.setDefaultButton(runButton);
		}
	    });
        quitButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    System.exit(0);
		}
	    });

        // put the buttons into the button panel:
        b.setLayout(new FlowLayout());
        b.add(runButton);
        b.add(pauseButton);
        b.add(resetButton);
        b.add(quitButton);

        // put the Table canvas and the button panel into the UI:
        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        setBorder(BorderFactory.createEmptyBorder(
						  externalBorder, externalBorder, externalBorder, externalBorder));
        add(t);
        add(b);

        // put the UI into the Frame
        pane.getContentPane().add(this);
        root = getRootPane();
        root.setDefaultButton(runButton);
    }
}
