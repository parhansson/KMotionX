/* 
 * Copyright 2014 (C) par.hansson@gmail.com 
 *  
 * Created on : 2014-03-01 
 * Author     : Pär Hansson 
 * 
 */

package com.dynomotion.kmotionx.viewer;



import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.Bullet;
import org.eclipse.swt.custom.LineStyleEvent;
import org.eclipse.swt.custom.LineStyleListener;
import org.eclipse.swt.custom.ST;
import org.eclipse.swt.custom.StyleRange;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.ShellAdapter;
import org.eclipse.swt.events.ShellEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GlyphMetrics;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;

import com.dynomotion.kmotionx.KMotion;
import com.dynomotion.kmotionx.callback.CompleteCallbackHandler;
import com.dynomotion.kmotionx.callback.StatusCallbackHandler;
import com.dynomotion.kmotionx.callback.UserCallbackHandler;

/**
 */
public class GCodeViewer {
  Shell shell;

  StyledText text;
  KMotion kmx;

  GCodeLineStyler lineStyler = new GCodeLineStyler();

  FileDialog fileDialog;
	public GCodeViewer() {
		kmx = new KMotion(true);
	}
  
  Menu createFileMenu() {
    Menu bar = shell.getMenuBar();
    Menu menu = new Menu(bar);
    MenuItem item;

    // Open
    item = new MenuItem(menu, SWT.CASCADE);
    item.setText("Open");
    item.setAccelerator(SWT.MOD1 + 'O');
    item.addSelectionListener(new SelectionAdapter() {
      public void widgetSelected(SelectionEvent event) {
        openFile();
      }
    });

    // Exit
    item = new MenuItem(menu, SWT.PUSH);
    item.setText("Exit");
    item.addSelectionListener(new SelectionAdapter() {
      public void widgetSelected(SelectionEvent e) {
        menuFileExit();
      }
    });
    return menu;
  }

  void createMenuBar() {
    Menu bar = new Menu(shell, SWT.BAR);
    shell.setMenuBar(bar);

    MenuItem fileItem = new MenuItem(bar, SWT.CASCADE);
    fileItem.setText("File");
    fileItem.setMenu(createFileMenu());

  }

  void createShell(Display display) {
    shell = new Shell(display);
    shell.setText("Window");
    GridLayout layout = new GridLayout();
    layout.numColumns = 1;
    shell.setLayout(layout);
    shell.addShellListener(new ShellAdapter() {
      public void shellClosed(ShellEvent e) {
        lineStyler.disposeColors();
        text.removeLineStyleListener(lineStyler);
      }
    });
  }

  void createStyledText() {
    text = new StyledText(shell, SWT.BORDER | SWT.MULTI | SWT.V_SCROLL
        | SWT.H_SCROLL);
    GridData spec = new GridData();
    spec.horizontalAlignment = GridData.FILL;
    spec.grabExcessHorizontalSpace = true;
    spec.verticalAlignment = GridData.FILL;
    spec.grabExcessVerticalSpace = true;
    text.setLayoutData(spec);
    text.addLineStyleListener(lineStyler);
    
    //text is your StyledText
    //Add linenumbers
    text.addLineStyleListener(new LineStyleListener()
    {
        public void lineGetStyle(LineStyleEvent e)
        {
            //Set the line number
            e.bulletIndex = text.getLineAtOffset(e.lineOffset);

            //Set the style, 12 pixles wide for each digit
            StyleRange style = new StyleRange();
            style.metrics = new GlyphMetrics(0, 0, Integer.toString(text.getLineCount()+1).length()*12);

            //Create and set the bullet
            e.bullet = new Bullet(ST.BULLET_NUMBER,style);
        }
    });    
    text.setEditable(true);
    Color bg = Display.getDefault().getSystemColor(SWT.COLOR_GRAY);
    text.setBackground(bg);
  }

  void displayError(String msg) {
    MessageBox box = new MessageBox(shell, SWT.ICON_ERROR);
    box.setMessage(msg);
    box.open();
  }

  public static void main(String[] args) {
    Display display = new Display();
    GCodeViewer example = new GCodeViewer();
    Shell shell = example.open(display);
    while (!shell.isDisposed())
      if (!display.readAndDispatch())
        display.sleep();
    display.dispose();
  }

  public Shell open(Display display) {
    createShell(display);
    createMenuBar();
    createStyledText();
    shell.setSize(500, 400);
    shell.open();
    return shell;
  }

  void openFile() {
    if (fileDialog == null) {
      fileDialog = new FileDialog(shell, SWT.OPEN);
    }

    fileDialog.setFilterExtensions(new String[] {"*.gcode","*.ngc", "*.*" });
    String name = fileDialog.open();

    open(name);
  }

  void open(String name) {
    final String textString;

    if ((name == null) || (name.length() == 0))
      return;

    File file = new File(name);
    if (!file.exists()) {
      String message = "Err file no exist";
      displayError(message);
      return;
    }

    try {
      FileInputStream stream = new FileInputStream(file.getPath());
      try {
        Reader in = new BufferedReader(new InputStreamReader(stream));
        char[] readBuffer = new char[2048];
        StringBuffer buffer = new StringBuffer((int) file.length());
        int n;
        while ((n = in.read(readBuffer)) > 0) {
          buffer.append(readBuffer, 0, n);
        }
        textString = buffer.toString();
        stream.close();
      } catch (IOException e) {
        // Err_file_io
        String message = "Err_file_io";
        displayError(message);
        return;
      }
    } catch (FileNotFoundException e) {
      String message = "Err_not_found";
      displayError(message);
      return;
    }
    // Guard against superfluous mouse move events -- defer action until
    // later
    Display display = text.getDisplay();
    display.asyncExec(new Runnable() {
      public void run() {
        text.setText(textString);
      }
    });

    // parse the block comments up front since block comments can go across
    // lines - inefficient way of doing this
    lineStyler.parseBlockComments(textString);
    kmx.Interpret(0, file.getAbsolutePath(), 0, -1, 1, new StatusCallbackHandler() {
		
		@Override
		public void handleStatus(final int line_no, final String msg) {
		      Display display = text.getDisplay();
		      display.asyncExec(new Runnable() {
		        public void run() {
		        	
		        	Color lc = Display.getDefault().getSystemColor(SWT.COLOR_DARK_CYAN);
		        	try{
		        		text.setLineBackground(line_no-1, 1, Display.getDefault().getSystemColor(SWT.COLOR_GRAY));
		        		text.setLineBackground(line_no, 1, lc);
		        		
		        	} catch (Exception e){
		        		
		        	}
		        }
		      });		      
		      
			
		}
	}, new CompleteCallbackHandler() {
		
		@Override
		public void handleComplete(int status, int line_no, int sequence_number,
				String err) {
			// TODO Auto-generated method stub
			
		}
	});
  }

  void menuFileExit() {
    shell.close();
  }
}

