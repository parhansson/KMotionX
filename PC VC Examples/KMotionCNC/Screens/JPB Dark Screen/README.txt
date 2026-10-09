JPB Dark Screen for KMotionCNC 5.4.5 and later
==============================================

Contents
  JPB Dark.scr        the screen (1920 x 1040)
  JPB Dark images\    the button images it uses

Install
  First rename "JPB Dark.scr.txt" to "JPB Dark.scr" (Gmail blocks .scr
  attachments, even inside a zip, so it travels with a .txt on the end).
  Keep "JPB Dark images" in the same folder as "JPB Dark.scr" (for example both
  under KMotion\KMotionCNC\Screens) and select JPB Dark.scr as the screen.
  The images are referenced by file name only, so KMotionCNC finds them in the
  screen's own folder and its subfolders - no paths to edit.

What each KMotionCNC version shows
  - Stock 5.4.5: the dark layout; the G-code editor, MDI box and edit boxes
    stay white.
  - With the GEditorEditComboSupportColorControl patch: editor, MDI box and
    edit boxes dark as well.
  - 5.5.1 and later: editor, MDI box and edit boxes dark, plus the screen's
    Thin Edges option (",ThinEdges:1" at the end of the Main: line, or the
    "Thin Edges" checkbox under Main Dialog Screen in the Screen Editor):
    the jog pad, Keyboard Jog, E-stop / Feed Hold / Cycle Start and both
    toolbars get the same thin 1px edge as the text buttons, filled with
    their own colors, and all button edges are shaded from each button's
    color instead of bright white. Earlier versions ignore it and draw their
    usual 2px edge; everything else looks the same.
  - The extra 5th value "1" in the image buttons' Colors fields was an
    earlier per-button form of Thin Edges. It is no longer used and is
    harmless (the Screen Editor drops it when a button's colors are edited).

Machine-specific parts
  The Standard / Knee Z / PCB buttons, the "CHOOSE init ->" prompt and the
  init / status readouts drive my own init-selection M-codes and KFLOP
  variables. They won't do anything useful on another machine; hide or
  repurpose them as needed. The C and A jog rows are for my 5-axis setup.

The dark button images are generated from KMotionCNC's own bitmaps by a script,
so any size or colour change can be regenerated. Happy to adjust anything.

Jim Barad
