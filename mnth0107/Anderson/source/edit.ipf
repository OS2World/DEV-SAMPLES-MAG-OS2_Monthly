.*********************************************************************
.*                                                                   *
.*        Help System for Edit (Simple Programmer's Editor)          *
.*                                                                   *
.*        Brian R. Anderson                                          *
.*        January 25, 1992                                           *
.*********************************************************************
:userdoc.
:title.Help for Edit Application
:body.
:h1 res=10.Extended Help
:i1 id=ed.Editor
.*
:p.
:hp2.Edit:ehp2. is a simple programmer's editor.  Its only advantage
over the standard OS/2 System Editor is that :hp2.Edit:ehp2.
displays line and column numbers -- something that is essential when
attempting to interpret compiler diagnostics.
:p.
All standard editing features are supported, including :hp4.undo/redo:ehp4.,
:hp4.cut:ehp4., :hp4.copy:ehp4., :hp4.paste:ehp4. (using the system
clipboard), :hp4.search/replace:ehp4., and :hp4.Go to line:ehp4..
:p.
Standard cursor control keys are available when editing.  CUA standards
are followed for :hp1.most:ehp1.  functions.
:p.
More help can be obtained on any of the main menu items (and their
associated pull-down menus) by double-clicking on one of the following:
:p.
:sl compact.
:li.:link res=100 reftype=hd.File:elink.
:li.:link res=200 reftype=hd.Edit:elink.
:li.:link res=300 reftype=hd.Help:elink.
:esl.
:p.
:sl compact.
:li.Brian R. Anderson
:li.British Columbia Institute of Technology
:li.3700 Willingdon Avenue
:li.Burnaby, B.C.  V5G 3H2
:li.CANADA
:esl.
:p.
.****************************************************************************
.*
:h1 res=100.File Menu Help
:i2 refid=ed.File Menu
.*
:p.
The File menu allows access to disk files.  :hp8.File size must not
exceed 64K.:ehp8.  To edit more than one file, start multiple copies of the
editor by double-clicking on the :hp2.Edit:ehp2. entry in a Desktop Manager
menu or from the command line:
:p.
:hp2.[C: \WORK]:ehp2. :hp7.START EDIT:ehp7.
:p.
With multiple copies of the editor running, you can use the clipboard
to move or copy text from one file to another (see :hp9.Edit:ehp9. menu for
more details).
:p.
For more information, double-click on one of these topics:
:p.
:sl compact.
:li.:link res=101 reftype=hd.New:elink.
:li.:link res=102 reftype=hd.Open:elink.
:li.:link res=103 reftype=hd.Save:elink.
:li.:link res=104 reftype=hd.Save as:elink.
:li.:link res=105 reftype=hd.Exit:elink.
:esl.
:p.
.***********************************************************************
.*
:h1 res=101.New Help
:i2 refid=ed.New
.*
:p.
:hp9.New:ehp9. clears the edit window so that you can create a new file.
If the contents of the edit window have changed since the last time you 
saved, you will be given a chance to save before the window is cleared.
:p.
Unless you want to discard the changes that you have made to your file,
select :hp2.Yes:ehp2. when prompted; if you select :hp2.No:ehp2., the
file will :hp5.not:ehp5. be saved.  If you select :hp2.Cancel:ehp2., you
will be returned to the edit window without any action.
:p.
To activate this command, select :hp9.New:ehp9. from the menu.
:p.
.***********************************************************************
.*
:h1 res=102.Open Help
:i2 refid=ed.Open
.*
:p.
:hp9.Open:ehp9. allows you to access existing files on any disk or within
any directory.  The dialog box that results from choosing :hp9.Open:ehp9.
contains two list boxes -- one with disks/directories, the other with files.
:p.
To change to another disk or directory, simply double-click on the disk or 
directory of interest -- the files list box will immediately show the
files in that directory.
:p.
To select a file, simply double-click on the required file and it will
be opened and read into the edit window.
:p.
You may also open files using just the keyboard:  Of course, you may
simply type the name of the file that you want to open, followed by the
Enter key.  Use the tab key to switch from one part of the dialog box
to another.  Use the up and down Arrow keys to scroll through the
disk/directory and file list boxes.  Use the Enter key to make your
selection.  Use the Escape key to cancel without making a selection.
:p.
To activate this command, select :hp9.Open:ehp9. from the menu.
:p.
If you select :hp9.Open:ehp9. when you are already editing a file (and
the file has changed since the last time you saved), you will be given
the opportunity to save before the new file is opened.
:p.
.***********************************************************************
.*
:h1 res=103.Save Help
:i2 refid=ed.Save
.*
:p.
:hp9.Save:ehp9. will save the contents of the edit window to the current
file.  If the edit window contains new text that has never been saved,
you will be prompted for a filename.  The current filename (if any)
appears in the title bar.
:p.
To activate this command, select :hp9.Save:ehp9. from the menu.
:p.
.***********************************************************************
.*
:h1 res=104.Save as Help
:i2 refid=ed.Save as
.*
:p.
:hp9.Save as:ehp9. allows you to save the contents of the edit window
under a different filename (i.e., different from previous saves and/or
different from the original filename).  Unlike :hp9.Save:ehp9., (which
saves using the existing name) :hp9.Save as:ehp9. :hp5.always:ehp5. prompts
for a new filename.
:p.
:hp9.Save as:ehp9. is useful for two distinct purposes: saving an altered
file under a different name (to preserve the original file); and
splitting a file.
:p.
Splitting a file is :hp1.tricky:ehp1., and should only be attempted after
first saving a backup copy of the entire file.  Here are the steps involved
in splitting a file:
:p.
:sl compact.
:li.1.  Cut one portion of the text to the clipboard.
:li.2.  Save the remaining text to a new file.
:li.3.  Delete the text that was just saved to the file.
:li.4.  Paste the text that was previously Cut to the clipboard.
:li.5.  Save the retreived text to a :hp5.second:ehp5. new file.
:esl.
:p.
To activate this command, select :hp9.Save as:ehp9. from the menu.
:p.
.***********************************************************************
.*
:h1 res=105.Exit Help
:i2 refid=ed.Exit
.*
:p.
:hp9.Exit:ehp9. terminates the editor session, and returns you to the
operating system (PM screen or OS/2 command prompt, depending upon how
you started the program).  If you have changed the contents of the
edit window since the last time that you saved, you will be given 
the opportunity to save the contents to a file (or Cancel and return
to the editor).
:p.
To activate this command (i.e., quit the editor), you may select
:hp9.Exit:ehp9. from the menu, select Close from the system menu,
click on the End Task pushbutton from the task list, or use the
keyboard accelerator: F3.
:p.
.***********************************************************************
.*
:h1 res=200.Edit Menu Help
:i2 refid=ed.Edit
.*
:p.
The :hp9.Edit:ehp9. menu provides various text manupulation functions.
For more information, double-click on one of these topics:
:p.
:sl compact.
:li.:link res=201 reftype=hd.Undo:elink.
:li.:link res=202 reftype=hd.Cut:elink.
:li.:link res=203 reftype=hd.Copy:elink.
:li.:link res=204 reftype=hd.Paste:elink.
:li.:link res=205 reftype=hd.Delete:elink.
:li.:link res=206 reftype=hd.Find:elink.
:li.:link res=207 reftype=hd.Replace:elink.
:li.:link res=208 reftype=hd.Go to line:elink.
:esl.
:p.
.***********************************************************************
.*
:h1 res=201.Undo Help
:i2 refid=ed.Undo
.*
:p.
:hp9.Undo:ehp9. provides a single-level undo/redo function to allow
you to recover from mistakes.  This menu selection will be
:hp1.greyed:ehp1.  (i.e., unavailable) if the editor does not have
an action that it can recover from.  For example, it is not possible
to undo a save.
:p.
:hp9.Undo:ehp9. is most useful when you accidently delete something
that you meant to keep.  It is important to realize that :hp2.Edit:ehp2.
can recover only the most recent action -- if you delete something and
don't realize it until after you have started typing, it is too late!
:p.
:hp9.Undo:ehp9. can also recover from an accidental undo -- i.e., it
can re-do.  Again, the same caution applies -- if you don't realize
that the undo was a mistake (and do something else), you will not
be able to re-do, because :hp2.Edit:ehp2. can undo only your latest action.
:p.
To activate this command, select :hp9.Undo:ehp9. from the menu, or
use the keyboard accelerator: Alt+Backspace.
:p.
.***********************************************************************
.*
:h1 res=202.Cut Help
:i2 refid=ed.Cut
.*
:p.
:hp9.Cut:ehp9. allows you to move text from one part of a document
to another, or even between two different documents.
:p.
:hp9.Cut:ehp9. (or :hp9.Copy:ehp9.) places text on the clipboard, where
it may be later retreived by :hp9.Paste:ehp9..  Text remains on the
clipboard even if you exit the program, and is available to other
processes (e.g., other running copies of :hp2.Edit:ehp2.).
:p.
To cut a section of text, you must first :hp1.highlight:ehp1.  the text.
You can do this either with the mouse or the keyboard.  With the mouse,
you point to the beginning of the area that you want to cut, depress and
hold the left mouse button, and then move the mouse to the end of the area
that you want to cut.  With the keyboard, you move the text cursor (with
the Arrow or Page Up/Page Down keys) to the beginning of the area that
you want to cut, depress (and hold) the Shift key and then move (using
the same keys as before) to the end of the area that you want to cut.
In either case (mouse or keyboard) the result is that the selected text 
is marked by changing colors (usually by making the text reverse video, 
but may be different if you have changed the default colors on your computer).
:p.
Once the text has been chosen (:hp1.highlighted:ehp1. ) select 
:hp9.Cut:ehp9. from the menu, or use the keyboard accelerator:
Shift+Del.
:p.
If no text has been selected, the :hp9.Cut:ehp9. menu item will be
:hp1.greyed:ehp1.  (i.e., disabled).
:p.
:hp9.Cut:ehp9. and :hp9.Copy:ehp9. are somewhat similar (in that both
result in the selected text being placed on the clipboard), but very
different (in that :hp9.Cut:ehp9. results in the text being deleted,
while :hp9.Copy:ehp9. does not).
:p.
.***********************************************************************
.*
:h1 res=203.Copy Help
:i2 refid=ed.Copy
.*
:p.
:hp9.Copy:ehp9. allows you to duplicate text from one part of a document
to another, or even between two different documents.
:p.
:hp9.Copy:ehp9. (or :hp9.Cut:ehp9.) places text on the clipboard, where
it may be later retreived by :hp9.Paste:ehp9..  Text remains on the
clipboard even if you exit the program, and is available to other
processes (e.g., other running copies of :hp2.Edit:ehp2.).
:p.
To copy a section of text, you must first :hp1.highlight:ehp1.  the text.
You can do this either with the mouse or the keyboard.  With the mouse,
you point to the beginning of the area that you want to copy, depress and
hold the left mouse button, and then move the mouse to the end of the area
that you want to copy.  With the keyboard, you move the text cursor (with
the Arrow or Page Up/Page Down keys) to the beginning of the area that
you want to copy, depress (and hold) the Shift key and then move (using the
same keys as before) to the end of the area that you want to copy.  In either
case (mouse or keyboard) the result is that the selected text is marked by
changing colors (usually by making the text reverse video, but may be
different if you have changed the default colors on your computer).
:p.
Once the text has been chosen (:hp1.highlighted:ehp1. ) select 
:hp9.Copy:ehp9. from the menu, or use the keyboard accelerator:
Ctrl+Ins.
:p.
If no text has been selected, the :hp9.Copy:ehp9. menu item will be
:hp1.greyed:ehp1.  (i.e., disabled).
:p.
:hp9.Copy:ehp9. and :hp9.Cut:ehp9. are somewhat similar (in that both
result in the selected text being placed on the clipboard), but very
different (in that :hp9.Cut:ehp9. results in the text being deleted,
while :hp9.Copy:ehp9. does not).
:p.
.***********************************************************************
.*
:h1 res=204.Paste Help
:i2 refid=ed.Paste
.*
:p.
:hp9.Paste:ehp9. works in conjunction with :hp9.Cut:ehp9. or :hp9.Copy:ehp9.
to either move or duplicate sections of text.  After text has been
:hp1.highlighted:ehp1.  and then placed on the clipboard, :hp9.Paste:ehp9.
retreives the text from the clipboard and inserts it into the edit window
at the current cursor position.
:p.
To activate this command, select :hp9.Paste:ehp9. from the menu, or
use the keyboard accelerator: Shift+Ins.
:p.
:hp9.Paste:ehp9. does :hp5.not:ehp5. remove the text from the clipboard
 -- i.e., the same text can be retreived multiple times.
:p.
If text is :hp1.highlighted:ehp1.  at the time that :hp9.Paste:ehp9. is 
selected, the clipboard data :hp5.replaces:ehp5. the highlighted text.
Although this :hp1.feature:ehp1.  can sometimes be useful, it is usually
a mistake -- fortunately, even this operation can be reversed by the
:hp9.Undo:ehp9. command.
:p.
.***********************************************************************
.*
:h1 res=205.Delete Help
:i2 refid=ed.Delete
.*
:p.
:hp9.Delete:ehp9. allows you to remove text from a document.  The text
does :hp5.not:ehp5. go to the clipboard, but can be retreived (if
done immediately) by :hp9.Undo:ehp9.
:p.
To delete a section of text, you must first :hp1.highlight:ehp1.  the text.
You can do this either with the mouse or the keyboard.  With the mouse,
you point to the beginning of the area that you want to delete, depress and
hold the left mouse button, and then move the mouse to the end of the area
that you want to delete.  With the keyboard, you move the text cursor (with
the Arrow or Page Up/Page Down keys) to the beginning of the area that
you want to delete, depress (and hold) the Shift key and then move (using the
same keys as before) to the end of the area that you want to delete.  In
either case (mouse or keyboard) the result is that the selected text is
marked by changing colors (usually by making the text reverse video, but may
be different if you have changed the default colors on your computer).
:p.
Once the text has been chosen (:hp1.highlighted:ehp1. ) select 
:hp9.Delete:ehp9. from the menu, or use the keyboard accelerator:
Del.
:p.
If no text has been selected, the :hp9.Delete:ehp9. menu item will be
:hp1.greyed:ehp1.  (i.e., disabled), however the Del key will still operate:
it deletes the current character.
:p.
.***********************************************************************
.*
:h1 res=206.Find Help
:i2 refid=ed.Find
.*
:p.
:hp9.Find:ehp9. allows you to search for one or more occurrences of a word
or phrase (up to about 50 characters).
:p.
To activate this command, select :hp9.Find:ehp9. from the menu, or use
the keyboard accelerator: Ctrl+F.
:p.
When activated, you will be prompted to enter a string to search for.
After typing in the target string, press the Enter key or click on the
Find pushbutton.  If the search is successful, the cursor will move
to the first occurrence of the string; the editor will also
:hp1.highlight:ehp1.  the string in the edit window.
:p.
If the target string is not found, a message box informs you of that.
:p.
If you do not specify a target string, :hp9.Find:ehp9. will exit 
without doing anything.  Depressing the Escape key or clicking on
the Cancel pushbutton has the same effect.
:p.
If you activate the :hp9.Find:ehp9. again, the previous word will
already appear in the target field, allowing you to more easily
continue the search (for multiple occurrences) -- just hit the
Enter key or click on the Find pushbutton.
:p.
:hp9.Find:ehp9. starts its search from the cursor position, and
continues to the end of the edit window.
:p.
.***********************************************************************
.*
:h1 res=207.Replace Help
:i2 refid=ed.Replace
.*
:p.
:hp9.Replace:ehp9. allows you to search, and optionally replace text
(a word or phrase up to about 50 characters).
:p.
To activate this command, select :hp9.Replace:ehp9. from the menu, or use
the keyboard accelerator: Ctrl+R.
:p.
When activated, you will be prompted to enter a string to search for.
(You may optionally enter the replacement string at this time as well).
After typing in the target string, press the Enter key or click on the
Find pushbutton.  If the search is successful, the cursor will move
to the first occurrence of the string; the editor will also
:hp1.highlight:ehp1.  the string in the edit window.
:p.
Once the first occurrence of the target is found, the :hp9.Replace:ehp9.
dialog box will reappear (this time with the Replace and Replace All
pushbuttons active).  You may now enter the replacement string (if you
leave it blank, the target is replaced with :hp5.nothing:ehp5. -- i.e., 
the target is deleted).  You may now choose Find (searches for next
occurrence :hp5.without replacing:ehp5.), Replace (replaces the highlighted
occurrence of the target), Replace All (replaces :hp5.every:ehp5. occurrence
of the target, or Cancel (does nothing).
:p.
If the target string is not found, a message box informs you of that.
:p.
In the case of Replace All, a message box informs you when it has
replaced all occurrences of the target.
:p.
If you do not specify a target string, :hp9.Replace:ehp9. will exit 
without doing anything.  Depressing the Escape key or clicking on
the Cancel pushbutton has the same effect.
:p.
:hp9.Replace:ehp9. starts its search from the cursor position, and
continues to the end of the edit window.
:p.
.***********************************************************************
.*
:h1 res=208.Go to line Help
:i2 refid=ed.Go to line
.*
:p.
:hp9.Go to line:ehp9. allows you to go directly to a specified line.
:p.
To activate this command, select :hp9.Go to line:ehp9. from the menu,
or use the keyboard accelerator: Ctrl+G.
:p.
This feature is particularly helpful when you are using compiler
diagnostic messages (which reports syntax errors by line number) to 
locate and fix program bugs.
:p.
You will find it easier if you :hp5.first:ehp5. fix the syntax errors
that occur near the end of the file, as changes made (added or deleted
lines) will not affect line numbering of syntax errors that occur near
the beginning of the file.
:p.
.***********************************************************************
.*
:h1 res=300.Help Menu Help
:i2 refid=ed.Help
.*
:p.
The Help menu gives you access to several types of help, including
help for using the help system, general help about this program,
a list of keys used by this program, an index of available
help topics, and copyright information about :hp2.Edit:ehp2..
:p.
For more information, double-click on one of these topics:
:p.
:sl compact.
:li.:link res=301 reftype=hd.Help for help:elink.
:li.:link res=302 reftype=hd.Extended help:elink.
:li.:link res=303 reftype=hd.Keys help:elink.
:li.:link res=304 reftype=hd.Help index:elink.
:li.:link res=305 reftype=hd.About:elink.
:esl.
:p.
.***********************************************************************
.*
:h1 res=301.Help for help Explained
:i2 refid=ed.Help for help
.*
:p.
:hp9.Help for help:ehp9. gives information about using the OS/2 help
system (known as the :hp1.Information Presentation Facility:ehp1. ).
The various features and services are explained, and the keystrokes
used by the help system are listed and explained.
:p.
.***********************************************************************
.*
.*
:h1 res=302.Extended help Explained
:i2 refid=ed.Extended help
.*
:p.
:hp9.Extended help:ehp9. is actually a rather abbreviated explanation
of this application.  Hypertext links (shown in light green) give you
access to other available help for this application (e.g., instructions for
using each of the menu options).
:p.
.***********************************************************************
.*
:h1 res=303.Keys help Explained
:i2 refid=ed.Keys help
.*
:p.
:hp9.Keys help:ehp9. lists the keystrokes used by this application.
:p.
.***********************************************************************
.*
:h1 res=304.Help index Explained
:i2 refid=ed.Help index
.*
:p.
:hp9.Help index:ehp9. give an alphebetic listing of all available help
topics.  The index is organized in two levels (main topic and subtopic).
All topics related to this application are subtopics of the main
topic :hp2.Editor:ehp2..
:p.
Each item in the index is a hypertext link: double-click or depress the
Enter key to get help on a topic in the index.
:p.
.***********************************************************************
.*
:h1 res=305.About Explained
:i2 refid=ed.About
.*
:p.
:hp9.About:ehp9. displays copyright information about this application.
:p.
.***********************************************************************
.*
:h1 res=310.Keys Help
:i2 refid=ed.Editing Keys
.*
:p.
:hp2.Editing Keys:ehp2.
:p.
:hp4.     
:dl tsize=8 break=all.     
:dt.F1
:dd.Help
:dt.F3
:dd.Exit
:dt.Alt+Backspace
:dd.Undo/Redo
:dt.Shift+Del
:dd.Cut
:dt.Ctrl+Ins
:dd.Copy
:dt.Del
:dd.Delete Block or Character
:dt.Ctrl+F
:dd.Find (Search)
:dt.Ctrl+R
:dd.Replace (Search and Replace)
:dt.Page Up
:dd.Scroll Text (Vertical)
:dt.Page Down
:dd.Scroll Text (Vertical)
:dt.Ctrl+Page Up
:dd.Scroll Text (Horizontal)
:dt.Ctrl+Page Down
:dd.Scroll Text (Horizontal)
:dt.Home
:dd.Beginning of Line
:dt.End
:dd.End of Line
:dt.Ctrl+Home
:dd.Beginning of Document
:dt.Ctrl+End
:dd.End of Document
:dt.Ctrl+Right Arrow
:dd.Next Word
:dt.Ctrl+Left Arrow
:dd.Previous Word
:dt.Insert
:dd.Toggle Insert/Overwrite
:dt.Del
:dd.Delete Character or Block
:edl.     
:ehp4.     
:p.
.***********************************************************************
:index.     
:euserdoc.     
.* end of help for Edit

