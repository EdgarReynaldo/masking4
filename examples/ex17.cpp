// Example 17: shows how you might organize your dialog hierarchy for the
// menu system of a simple game.

#include "../include/MASkinG.h"
using namespace MAS;


// Interface for our game menu dialogs. All our dialogs will be derived
// from this class and will at least implement the Run() function.
class GameDialog : public Dialog {
	public:
		// This function runs the dialog and returns a unique ID of
		// the dialog that's supposed to be run next (i.e. the next
		// state of the finite state machine that is our menu system).
		virtual int Run() = 0;
	
};


// These are the states the menu system can be in. Every state has a
// matching dialog that is run in that state. The exception is S_EXIT_GAME
// which is a special exit state.
#define		S_EXIT_GAME		-1
#define		S_MAIN_MENU		0
#define		S_NEW_GAME		1
#define		S_LOAD_GAME		2
#define		S_OPTIONS		3
#define		S_HIGHSCORES	4
#define		S_HELP			5
#define		S_CREDITS		6
#define		S_NEW_PLAYER	7
#define		S_PLAYING		8

// The number of states in the FSM
#define		S_COUNT			9

//------------------------------------------------------------------------------------

// Now come all the dialogs - one for each state. Note that in a real program
// you would probably want to put each in its own pair of header and implementation
// files. Also you would have to make some way for the dialogs to communicate
// between each other. Perhaps have a global game state structure or class
// that would record the current state of the game.


// The main (root) menu: just a few buttons that will put the menu system into
// a new state.
class MainMenu : public GameDialog {
	private:
		Wallpaper desktop;
		Label title;
		Button b[7];
		
	public:
		MainMenu() : GameDialog() {
			// first add a desktop
			Add(desktop);
			
			int dy = 12;
			int y = 3-dy;
			int h = 10;
			int x = 10;
			int w = 80;

			// make a title
			title.Shape(x, y+=dy, w, h, 1);
			title.ClearFlag(D_AUTOSIZE);
			title.AlignCentre();
			title.SetText("This is my uber cool game");
			Add(title);
			
			// add all the buttons; note that the buttons make the dialog exit when pushed
			char *bTitle[] = { "New Game", "Load Game", "Options", "Highscores", "Help", "Credits", "Exit" };
			for (int i=0; i<7; i++) {
				b[i].Shape(x, y+=dy, w, h, 1);
				b[i].SetText(bTitle[i]);
				b[i].MakeExit();
				Add(b[i]);
			}
			
			GiveFocusTo(&b[0]);
		}
		
		int Run() {
			// run the dialog and remember which object caused it to close
			Widget *obj = Execute();
			
			// find out which button was pushed and return the ID of the matching next state
			for (int i=0; i<6; i++) {
				if (obj == &b[i]) {
				GiveFocusTo(&b[i]);
					return i+1;
				}
			}
			
			return S_EXIT_GAME;
		}
};


// The new game dialog. Presents a list of players with an option to add new ones
// and has play and cancel buttons.
class NewGame : public GameDialog {
	private:
		Wallpaper desktop;
		Label title;
		ListBox lbPlayer;
		Button bAdd, bPlay, bCancel;
	
	public:
		NewGame() : GameDialog() {
			// add desktop
			Add(desktop);

			int dy = 12;
			int y = 6-dy;
			int h = 10;
			int x = 10;
			int w = 80;

			// make a title
			title.Shape(x, y+=dy, w, h, 1);
			title.ClearFlag(D_AUTOSIZE);
			title.AlignCentre();
			title.SetText("Select player");
			Add(title);

			// setup the listbox; note that the player names are setup every time the
			// dialog is run
			lbPlayer.Shape(x, y+=dy, w, 4*h, 1);
			lbPlayer.MakeExit();
			Add(lbPlayer);
			y+=3*h+dy;
			
			// the add player button
			bAdd.Shape(x, y, w, h, 1);
			bAdd.SetText("Add player");
			bAdd.MakeExit();
			Add(bAdd);
			
			// the play button
			bPlay.Shape(x, y+=dy, w, h, 1);
			bPlay.SetText("Play");
			bPlay.MakeExit();
			if (lbPlayer.Size() <= 0) bPlay.Disable();
			Add(bPlay);
			
			// the cancel button
			bCancel.Shape(x, y+=dy, w, h, 1);
			bCancel.SetText("Cancel");
			bCancel.MakeExit();
			Add(bCancel);
		}
		
		~NewGame() {
			// Remember which player was selected when the dialog is destroyed.
			push_config_state();
			set_config_file("gameframework.cfg");
			if (lbPlayer.Size() > 0) {
				set_config_int("PLAYERS", "selected", lbPlayer.Selection());
			}
			pop_config_state();
		}
		
		int Run() {
			// Setup the listbox with the player names. Names are read from a simple
			// Allegro config file. In a real game you would probably some other method
			// of storing players.
			lbPlayer.DeleteAllItems();
			push_config_state();
			set_config_file("gameframework.cfg");
			int nPlayers = get_config_int("PLAYERS", "count", 0);
			char tmp[64];
			for (int i=0; i<nPlayers; i++) {
				usprintf(tmp, "PLAYER%d", i+1);
				lbPlayer.InsertItem(new ListItemString(get_config_string(tmp, "name", "wtf?")), i);
			}
			if (nPlayers) {
				lbPlayer.Select(get_config_int("PLAYERS", "selected", 0));
				bPlay.Enable();
			}
			pop_config_state();

			// run the dialog
			Widget *obj = Execute(bPlay.Disabled() ? &bAdd : &bPlay);
			
			// determine what state we should go to next
			if (obj == &bAdd) {
				return S_NEW_PLAYER;
			}
			else if (obj == &bPlay || obj == &lbPlayer) {
				return S_PLAYING;
			}
			else if (obj == &bCancel) {
				return S_MAIN_MENU;
			}
			
			return S_MAIN_MENU;
		}
};


// The dialog for making a new player. This dialog allows you to just enter a name
// for the player. In a real game you would make a larger dialog with options to
// select various other plyer settings, depending on the nature of the game.
class NewPlayer : public GameDialog {
	private:
		Wallpaper desktop;
		Label title;
		Label lName;
		EditBox ebName;
		Button bAdd, bCancel;
	
	public:
		NewPlayer() : GameDialog() {
			Add(desktop);

			int dy = 12;
			int y = 26-dy;
			int h = 10;
			int x = 10;
			int w = 80;

			title.Shape(x, y+=dy, w, h, 1);
			title.ClearFlag(D_AUTOSIZE);
			title.AlignCentre();
			title.SetText("Add new player");
			Add(title);
			
			lName.Shape(x, y+=dy, w/4-2, h, 1);
			lName.ClearFlag(D_AUTOSIZE);
			lName.AlignCentre();
			lName.SetText("name");
			Add(lName);

			ebName.Shape(x+w/4+2, y, 3*w/4-2, h, 1);
			ebName.SetText("New Player", 30);
			ebName.MakeExit();
			Add(ebName);

			bAdd.Shape(x, y+=dy, w, h, 1);
			bAdd.SetText("Add");
			bAdd.MakeExit();
			Add(bAdd);

			bCancel.Shape(x, y+=dy, w, h, 1);
			bCancel.SetText("Cancel");
			bCancel.MakeExit();
			Add(bCancel);
		}

		int Run() {
			Widget *obj = Execute(&ebName);
			
			// Unless the cancel button was pushed to close the dialog, we have to
			// write the new player to the config file after closing.
			if (obj == &bAdd || obj == &ebName) {
				push_config_state();
				set_config_file("gameframework.cfg");
				int nPlayers = get_config_int("PLAYERS", "count", 0) + 1;
				char tmp[64];
				usprintf(tmp, "PLAYER%d", nPlayers);
				set_config_string(tmp, "name", ebName.GetText());
				set_config_int("PLAYERS", "count", nPlayers);
				set_config_int("PLAYERS", "selected", nPlayers);
				pop_config_state();
			}
			
			return S_NEW_GAME;
		}
};


// Placeholder for dialogs that have not yet been implemented.
class DialogNYI : public GameDialog {
	private:
		Wallpaper desktop;
		Label title;
		Button bBack;
	
	public:
		DialogNYI() : GameDialog() {
			Add(desktop);

			int dy = 12;
			int y = 38-dy;
			int h = 10;
			int x = 10;
			int w = 80;

			title.Shape(x, y+=dy, w, h, 1);
			title.ClearFlag(D_AUTOSIZE);
			title.AlignCentre();
			title.SetText("NOT YET IMPLEMENTED");
			Add(title);

			bBack.Shape(x, y+=dy, w, h, 1);
			bBack.SetText("Back");
			bBack.MakeExit();
			Add(bBack);
		}

		int Run() {
			Widget *obj = Execute(&bBack);
			return S_MAIN_MENU;
		}
};


//------------------------------------------------------------------------------------

// Now comes the core of our simple game menu system. The system is a finite state
// machine. It has a number of states where each has a dialog attached (except for
// the exit state). At all times the control is at exactly one of the states. For
// every iteration through the FSM the current state is evaluated and control is
// passed to the next state as indicated by the current state's return value.

void GameFramework() {
	// make all our states (dialogs)
	GameDialog *dlg[S_COUNT];
	dlg[S_MAIN_MENU] = new MainMenu;
	dlg[S_NEW_GAME] = new NewGame;
	dlg[S_LOAD_GAME] = new DialogNYI;
	dlg[S_OPTIONS] = new DialogNYI;
	dlg[S_HIGHSCORES] = new DialogNYI;
	dlg[S_HELP] = new DialogNYI;
	dlg[S_CREDITS] = new DialogNYI;
	dlg[S_NEW_PLAYER] = new NewPlayer;
	dlg[S_PLAYING] = new DialogNYI;

	// give control to the entry point (the main menu)
	GameDialog *currentDialog = dlg[S_MAIN_MENU];
	
	// do the state machine in an endless loop
	while (true) {
		// evaluate the current state and see what it wants
		int nextState = currentDialog->Run();
		
		// if the next state is the exit state, break from the loop
		if (nextState == S_EXIT_GAME) {
			break;
		}
		// otherwise pass the control to the next state
		else {
			currentDialog = dlg[nextState];
		}
	}
	
	// clean up when we're done
	for (int i=0; i<S_COUNT; i++) {
		delete dlg[i];
	}
}


// the standard MASkinG main function
int main() {
	Error err = InstallMASkinG("allegro.cfg");
	if (err) {
		err.Report();
	}
	
	GameFramework();
	
	ExitMASkinG();
	return 0;
}
END_OF_MAIN();
