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
	System.err.println("Placing " + this.getName() + " at " + cx + " " + cy);
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

    private  Map<Philosopher,Boolean> receivedByPhilosopher =
	new HashMap<Philosopher,Boolean>();

    public void setReceived(Philosopher p, boolean b) {
	receivedByPhilosopher.put(p, b);
    }
    
    public boolean getWhetherReceived(Philosopher p) {
	return receivedByPhilosopher.get(p);
    }

    public String getName() {
	return "Fork " + (this + "").substring(5, 8);
    }

    public void release() {
        reset();
    }

    public void reset() {
	System.err.print(this.getName());
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
    
    public void send() {
	status = Status.SENT;
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
	System.err.println(" reset to " + x + " " + y);
	clear();
	x = orig_x;
	y = orig_y;
	t.repaint();
        
    }

    // arguments are coordinates of acquiring philosopher's center
    //
    public void acquire(int px, int py) {
	
	if (!(t.getGraphics() == null))
	    clear();
        x = (orig_x + px)/2;
        y = (orig_y + py)/2;
	t.repaint();
    }
    
    // render self
    public void draw(String name, Graphics g) {
	g.drawString(name.contains("Bottle") ? name.substring(6) : name.substring(4), x, y);
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

}

class Diner extends Thread {

}

class Philosopher extends Thread {
    private static final Color THINK_COLOR = Color.blue;
    private static final Color WAIT_COLOR = Color.red;
    private static final Color EAT_COLOR = Color.green;
    private static final Color DRINK_COLOR = EAT_COLOR;
    private static final Color TRANQUIL_COLOR = THINK_COLOR;
    private static final double THINK_TIME = 4.0;
    private static final double TRANQUIL_TIME = THINK_TIME;
    private static final double FUMBLE_TIME = 2.0;
    // time between becoming hungry and grabbing first fork
    private static final double EAT_TIME = 3.0;

    private Coordinator c;
    private Table t;
    private static final int XSIZE = 50;
    private static final int YSIZE = 50;
    private int x;
    private int y;

    public Map<Fork,Boolean> forks = new HashMap<Fork,Boolean>();
    private Map<Bottle, Boolean> bottles = new HashMap<Bottle, Boolean>();
    
    private Map<Philosopher, Boolean> neighbors;
    private Random prn;
    private Color color;

    
    private Map<Fork,Boolean> hasForkRequestToken = new HashMap<Fork,Boolean>();
    private Map<Bottle, Boolean> hasBottleRequestToken = new HashMap<Bottle, Boolean>();
    private Map<Bottle, Boolean> needsBottle = new HashMap<Bottle, Boolean>();
    public void printForks() {
	System.err.println(this.getPhilosopherName() + "'s Forks");
	for (Fork f : forks.keySet().toArray(new Fork[0])) {
	    System.err.println(this.getPhilosopherName() + " has " + f.getName() + "? " + forks.get(f));
	}
    }
    
    public String getPhilosopherName() {
    	return "Philosopher " + (this + "").substring(14, 15);
    }
    private enum EatStatus {
	THINKS, HUNGRY, EATS;
    }

    private enum DrinkStatus {
	TRANQUIL, DRINKS, THIRSTY;
    }
    
    EatStatus eatStatus;
    DrinkStatus drinkStatus;
    public void addFork(Fork f, Boolean b) {
	forks.put(f, b);
    }

    public void addBottle(Bottle bottle, Boolean bool) {
	bottles.put(bottle, bool);
    }

    // Drinking Actions

    private void requestBottle(Bottle bottle) {
	if (drinkStatus == DrinkStatus.THIRSTY && needsBottle.get(bottle) && hasBottleRequestToken.get(bottle) && !bottles.get(bottle)) {
	    bottle.sendRequest();
	    hasBottleRequestToken.put(bottle, false);
	} else {
	    System.err.println(this.getPhilosopherName() + " doesn't request bottle " + bottle.getName());
	}
    }

    private void sendBottle(Bottle bottle) {
	if (hasBottleRequestToken.get(bottle) && bottles.get(bottle) && !(needsBottle.get(bottle) && (drinkStatus == DrinkStatus.DRINKS || forks.get(bottle.getCorrespondingFork())))) {
	    bottle.send();
	    bottles.put(bottle, false);
	} else {
	    System.err.println(this.getPhilosopherName() + " doesn't send bottle " + bottle.getName());
	}
    }

    private boolean isRequestSentToMe(Bottle bottle) {
	return bottle.isRequestSent() && !hasBottleRequestToken.get(bottle);
    }

    // if bottle's status is sent and this Philosopher doesn't have the bottle,
    // then it must have been sent by the other Philosopher. 
    private boolean isSentToMe(Bottle bottle) {
	return bottle.isSent() && !bottles.get(bottle);
    }
    
    private void receiveBottleRequest(Bottle bottle) {
	if (isRequestSentToMe(bottle)) {
	    hasBottleRequestToken.put(bottle, true);
	    bottle.receiveRequest();
	}
    }

    private void receiveBottle(Bottle bottle) {
	if (isSentToMe(bottle)) {
	    bottles.put(bottle, true);
	    bottle.receive();
	}
    }
	
    // Dining Actions
    private void requestFork(Fork f) {
	if (eatStatus == EatStatus.HUNGRY && hasForkRequestToken.get(f) && !forks.get(f)) {
	    
	    System.out.println(this.getPhilosopherName() + " requests " + f.getName());
	    f.sendRequest();
	    hasForkRequestToken.put(f, false);
	} else {
	    //System.out.println(this.getPhilosopherName() + " doesn't request " + (f == leftFork ? "left fork" : "rightFork"));
	}
    }

    private void releaseFork(Fork f) {
	if (eatStatus != EatStatus.EATS && hasForkRequestToken.get(f) && f.isDirty()) {
	    System.err.println(this.getPhilosopherName() + " sends fork " + f.getName());
	    f.send();
	    f.setClean();
	    forks.put(f, false);
	    //f.setReceived(this, false);
	    f.reset();
	} else {
	    System.err.println(this.getPhilosopherName() + " eating status " + eatStatus);
	    System.err.println(this.getPhilosopherName() + " has request token for " + f.getName() + " " + hasForkRequestToken.get(f));
	    System.err.println(f.getName() + " dirty? " + f.isDirty());
	    System.err.println(this.getPhilosopherName() + " doesn't send fork " + f.getName());
	}
    }

    private void receiveForkRequest(Fork f) {
	if (isRequestSentToMe(f)) {
	    f.receiveRequest();
	    hasForkRequestToken.put(f, true);
	}
    }

    private boolean isRequestSentToMe(Fork f) {
	return f.isRequestSent() && !hasForkRequestToken.get(f);
    }
    // if fork's status is sent and this philosopher doesn't have
    // the fork then it must have been sent by the other philosopher
    private boolean isSentToMe(Fork f) {
	return f.isSent() && !forks.get(f);
    }
    private void receiveFork(Fork f) {
	if (isSentToMe(f)) {
	    f.receive();
	    f.setClean();
	    forks.put(f, true);
	}
    }
    
    
    
    private void haveFork(Fork f) {
        
	forks.put(f, true);
	f.acquire(x, y);
	System.err.println("acquires fork");
	//System.out.println(f.getName() + " dirty? " + f.getWhetherDirty());
	f.setClean();
	
	
    }

    public void addNeighbor(Philosopher p, Boolean b) {
	neighbors.put(p, b);
    }
    // Constructor.
    // cx and cy indicate coordinates of center
    // Note that fillOval method expects coordinates of upper left corner
    // of bounding box instead.
    //
    public Philosopher(Table T, int cx, int cy,
                       Coordinator C) {
	t = T;
        x = cx;
        y = cy;
        forks = new HashMap<Fork, Boolean>();
	bottles = new HashMap<Bottle, Boolean>();
        c = C;
        prn = new Random();
        color = THINK_COLOR;

	neighbors = new HashMap<Philosopher, Boolean>();
	
	eatStatus = EatStatus.THINKS;
	
    }

    // start method of Thread calls run; you don't
    //

    public void run() {
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
    
    private void setNoBottlesNeeded() {
	for (Bottle bottle : bottles.keySet().toArray(new Bottle[0])) {
	    needsBottle.put(bottle, false);
	}
	System.err.println(this.getPhilosopherName() + " needs no bottles");
    }
    
    private void tranquil() throws ResetException {
	drinkStatus = DrinkStatus.TRANQUIL;
	setNoBottlesNeeded();
	System.out.println(this.getPhilosopherName() + " is tranquil");
	color = TRANQUIL_COLOR;
	t.repaint();
        delay(TRANQUIL_TIME);
    }

    private void needRandomSetOfBottles() {
	Random random = new Random();
	int numBottlesNeeded = random.nextInt(bottles.keySet().size());
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
    }

    private boolean allBottlesReceived() {
	for (boolean isReceived : bottles.values()) {
	    if (!isReceived) {
		return false;
	    }
	}
	return true;
    }

    private void requestBottles() {
	for (Bottle bottle : bottles.keySet().toArray(new Bottle[0])) {
	    requestBottle(bottle);
	    yield();
	}
	System.err.println(this.getPhilosopherName() + " has requested all bottles");
    }
    
    private void thirst() throws ResetException {
	drinkStatus = DrinkStatus.THIRSTY;
	needRandomSetOfBottles();
	color = WAIT_COLOR;
        t.repaint();
	System.out.println(this.getPhilosopherName() + " is thirsty");
        delay(FUMBLE_TIME);
	requestBottles();
	while (!allBottlesReceived()) {} // be hungry until all forks received          
    }

    private void drink() throws ResetException {
	drinkStatus = DrinkStatus.DRINKS;
	System.out.println(this.getPhilosopherName() + " drinks");
	color = DRINK_COLOR;
	t.repaint();
        delay(EAT_TIME);
    }

    public void addForkRequestToken(Fork f, boolean bool) {
	hasForkRequestToken.put(f, bool);
    }

    public void addBottleRequestToken(Bottle bottle, boolean bool) {
	hasBottleRequestToken.put(bottle, bool);
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
    
    private void think() throws ResetException {
	eatStatus = EatStatus.THINKS;
	System.out.println(this.getPhilosopherName() + " thinks");
	color = THINK_COLOR;
	t.repaint();
        delay(THINK_TIME);
	while (drinkStatus != DrinkStatus.THIRSTY) {} 
    }
    
    private void requestForks() {
	for (Fork f : forks.keySet().toArray(new Fork[0])) {
	    requestFork(f);
	    yield();
	}
	System.err.println(this.getPhilosopherName() + " has requested all forks");
    }
    private boolean allForksReceived() {
	for (boolean isReceived : forks.values()) {
	    if (!isReceived) {
		return false;
	    }
	}
	System.err.println(this.getPhilosopherName() + " has received all forks");
	return true;
    }
    private void hunger() throws ResetException {
	eatStatus = EatStatus.HUNGRY;
	color = WAIT_COLOR;
        t.repaint();
	System.out.println(this.getPhilosopherName() + " is hungry");
        delay(FUMBLE_TIME);
	requestForks();
	while (!allForksReceived()) {} // be hungry until all forks received          
    }

    private void dirtyMyForks() {
	for (Fork fork : forks.keySet().toArray(new Fork[0])) {
	    fork.setDirty();
	}
    }

    private void releaseMyForks() {
        for (Fork fork : forks.keySet().toArray(new Fork[0])) {
	    releaseFork(fork);
	    yield();
	}
	
	System.err.println(this.getPhilosopherName() + " has released all forks");
    }

    private void eat() throws ResetException {
	eatStatus = EatStatus.EATS;
	System.out.println(this.getPhilosopherName() + " eats");
	color = EAT_COLOR;
	t.repaint();
	dirtyMyForks();
        delay(EAT_TIME);
	while (drinkStatus == DrinkStatus.THIRSTY) {}
	releaseMyForks();
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
    private  Map<Philosopher, Map<Philosopher, Boolean>> neighbors =
	new HashMap<Philosopher, Map<Philosopher, Boolean>>();

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
		bottles[i].draw(g);
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
				    (int) (CANVAS_SIZE/2.0 + CANVAS_SIZE/6.0 * Math.cos(angle) + 10),
				    (int) (CANVAS_SIZE/2.0 - CANVAS_SIZE/6.0 * Math.sin(angle)) + 10);
		bottles[i] = new Bottle(this,
					(int) (CANVAS_SIZE/2.0 + CANVAS_SIZE/6.0 * Math.cos(angle)),
					(int) (CANVAS_SIZE/2.0 - CANVAS_SIZE/6.0 * Math.sin(angle)));
		 
	    }
	    
	    for (int i = 0; i < NUM_PHILS; i++) {
		double angle = Math.PI/2 + 2*Math.PI/NUM_PHILS*i;
	    
		philosophers[i] = new Philosopher(this,
						  (int) (CANVAS_SIZE/2.0 + CANVAS_SIZE/3.0 * Math.cos(angle)),
						  (int) (CANVAS_SIZE/2.0 - CANVAS_SIZE/3.0 * Math.sin(angle)),
						  c);            
	    }

	
	    int philosopherCount = 0;
	    for (Object l : Arrays.copyOfRange(lines, 2, lines.length)) {
		for (String neighborString : l.toString().split(" ")) {
		    int neighborInt = Integer.parseInt(neighborString.substring(0, 1));
		    philosophers[philosopherCount].addFork(forks[neighborInt], ifStarred(neighborString) ? false : true);
		    philosophers[philosopherCount].addForkRequestToken(forks[neighborInt], ifStarred(neighborString) ? true : false);
		    philosophers[philosopherCount].addBottle(bottles[neighborInt], ifStarred(neighborString) ? false : true);
		    philosophers[philosopherCount].addBottleRequestToken(bottles[neighborInt], ifStarred(neighborString) ? true : false);
		}
		philosopherCount++;
	    }

	  
	    System.err.println("\n\nStart");
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
