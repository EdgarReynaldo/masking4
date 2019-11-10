// example 21: Shows how to use multi column listboxes. Doesn't really use all the
// functionalities of ListBoxEx, but most are covered. We will create a simple
// dialog with three listboxes. The first will be a fullblown multicolumn listbox
// containing some meaningless numbers. We will also have checkboxes that will
// turn various features of the listbox on and off. The second will be quite the
// opposite: it will be a simple singlecolumn list with no fancy extra features.
// The example will show that the API for setting such a list and using it is
// really simple. The third list will be nothing special, it will just contain
// some multicolumn items. What is special about it though, is the fact that it
// will use a custom item class that will implement a custom way of sorting
// the list.

#ifdef  _MSC_VER
 #pragma warning(disable:4251) //warning C4251: 'xx' : class 'std::basic_string< ... >' needs to have dll-interface to be used by clients of class 'yy'
 #pragma warning(disable:4786) //identifier was truncated to '255' characters in the debug information
#endif

#include "../include/MASkinG.h"
using namespace MAS;

// The main dialo: quite simple, just a few lists, checkboxes and a couple of
// other widgets.
class MyDialog : public Dialog {
	private:
		// This is the custom list box item that will be used in the third
		// list. All it does is implement the less than operator slightly
		// differently than the base Item class does. This operator is used
		// for sorting items that are in the list so by changing it, we're
		// effectively changing the way how sorting works. The third list
		// will have three columns. The first two will contain text while
		// the third will contain numbers. The less than operator will,
		// instead of text, which is the default, compare numbers when
		// sorting by the third column is selected.
		class CustomItem : public ListBoxEx::Item {
			public:
				bool operator<(const class ListBoxEx::Item &rhs) const;
		};

	private:
		ClearScreen desktop;
		Panel panel;
		CheckBox chkHeader1, chkSort1, chkResize1, chkMulti1, chkSingle1, chkGrid1;
		ListBoxEx list1, list2, list3;
		Label lblSelected2;
		EditBox txtSelected2;
		Button btnExit;

	public:
		MyDialog();
		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0);
};


// constructor
MyDialog::MyDialog() : Dialog() {
	// Setup the desktop.
	Add(desktop);

	// Setup a panel to put all the other widgets on. Just so it looks better.
	panel.Shape(4, 4, SCREEN_W - 8, SCREEN_H - 44);
	Add(panel);

	// Setup all the checkboxes for the first list:

	// Should the header be visible?
	chkHeader1.Setup(16, 12, 120, 16, 0, 0, "Header");
	chkHeader1.Select();	// by default the header is visible, so we select the checkbox
	Add(chkHeader1);

	// Should clicking on a column header sort the list?
	chkSort1.Setup(16, 32, 120, 16, 0, 0, "Auto sort");
	Add(chkSort1);

	// Should the columns be resizable?
	chkResize1.Setup(16, 52, 120, 16, 0, 0, "Column resizing");
	chkResize1.Select();
	Add(chkResize1);

	// Does the list allow multiple items to be selected?
	chkMulti1.Setup(176, 12, 120, 16, 0, 0, "Multiselect");
	Add(chkMulti1);

	// Does the list activate on single click or double click?
	chkSingle1.Setup(176, 32, 120, 16, 0, 0, "Single click");
	Add(chkSingle1);

	// Should the list draw gridlines or not?
	chkGrid1.Setup(176, 52, 120, 16, 0, 0, "Grid lines");
	Add(chkGrid1);

	// Setup the first list:
	list1.Shape(16, 72, 320, 320);

	int i;

	// The list will have six columns titled "col 1", "col 2" and so on
	for (i=0; i<6; i++) {
		char tmp[64];
		usprintf(tmp, "col %d", i+1);

		// Insert the column at position i, with centre alignment of the
		// column title and make it 80 pixels wide.
		list1.InsertColumn(tmp, i, 2, 80);
	}

	// The list will have 100 items, each will contain its coordinates.
	for (i=0; i<100; i++) {
		// Make new item. Note that once a new item is inserted in a list,
		// the list owns it and is responsible for deleting it when the
		// time comes, so we don't have to remember the pointer and delete
		// it later. In fact, we mustn't.
		list1.InsertItem(new ListBoxEx::Item());

		// Setup each column in the current item: just write coordinates in it.
		for (int j=0; j<6; j++) {
			char tmp[64];
			usprintf(tmp, "(%d, %d)", i, j);

			// We could use something like list1.GetItem(i)->SetText(tmp, j), or
			// we could remember the pointer we created when inserting the item
			// and use that to enter the text, but calling SetItem() is just so
			// much easier.
			list1.SetItem(i, j, tmp);
		}
	}

	Add(list1);

	// Setup the second list. This is a simple single column list. Notice
	// how wasy it is to set it up. We could actually use the same process
	// as with the first list, but InsertItem(string) is there just for
	// these case when you don't need any special features and don't want
	// to be bothered with the details.
	list2.Shape(360, 16, 120, 120);
	list2.InsertItem("zero");
	list2.InsertItem("one");
	list2.InsertItem("two");
	list2.InsertItem("three");
	list2.InsertItem("four");
	list2.InsertItem("five");
	list2.InsertItem("six");
	list2.InsertItem("seven");
	list2.InsertItem("eight");
	list2.InsertItem("nine");
	list2.InsertItem("ten");
	Add(list2);

	// We'll have a simple editbox to show which item of the second list
	// is currently selected. The editbox will be updated every time the
	// list selection changes.

	lblSelected2.Setup(360, 144, 120, 16, 0, 0, "selected item:", 0);
	Add(lblSelected2);

	txtSelected2.Setup(360, 164, 120, 24, 0, D_READONLY, "0 (zero)", 64);
	Add(txtSelected2);

	// Setup the third list. This is similar to list1 except that we will
	// want to be able to sort it. Because one of the columns will contain
	// numerical values and we'll want to sort by those, we need to use
	// the custom list item class that implements this.
	list3.Shape(360, 200, 240, 180);

	// The list will have three columns: first and last name and age
	list3.InsertColumn("first name");
	list3.InsertColumn("last name");
	list3.InsertColumn("age");

	// Create some items. Notice how we use CustomItem instead of Item.

	int nItems = 7;
	const char *first[] = { "Johnny", "Homer", "Bart", "Mickey", "Donald", "Buggs", "Wile E" };
	const char *last[] = { "Bravo", "Simpson", "Simpson", "Mouse", "Duck", "Bunny", "Coyote" };
	const char *age[] = { "18", "37", "12", "31", "27", "5", "6" };

	for (i=0; i<nItems; i++) {
		list3.InsertItem(new CustomItem());
		list3.SetItem(i, 0, first[i]);
		list3.SetItem(i, 1, last[i]);
		list3.SetItem(i, 2, age[i]);
	}

	// make sure the list can be sorted
	list3.EnableAutosort(true);
	Add(list3);

	// setup the exit button
	btnExit.Setup(8, SCREEN_H-32, 120, 24, KEY_Q, D_EXIT, "&Quit");
	Add(btnExit);
}


// Custom item's less than operator, used when sorting items. We know this item
// will have a numerical value in the third column (age) so when sorting by
// that column is selected, we compare using the numerical value of the items,
// otherwise we let the default implementation do the work for us.
// The operator must compare this item with the item that is passed as the
// argument and return true if this item is smaller, false otherwise.
bool MyDialog::CustomItem::operator<(const class ListBoxEx::Item &rhs) const {
	// Which column are we comparing? Note that sortColumn is a member variable
	// of the Item class.
	if (sortColumn == 2) {
		// Convert texts in the third columns of the two items to numbers,
		// compare them and map them to a positive number (1) if the first
		// is smaller or a negativenumber (-1) otherwise.
		int tmp = (uatof(GetText(sortColumn)) < uatof(rhs.GetText(sortColumn))) ? 1 : -1;

		// sortOrder is a member variable of the item class and determines the
		// sort order. It is a positive number (1) if ascending order is requested
		// and a negative number (-1) for descending order (i.e. reverse sorting).
		// We have to multiply tmp with this number. If we multiply with 1,
		// nothing changes, but if we multiply with -1, the sort order is reversed,
		// which is just what we want.
		tmp *= sortOrder;

		// If this item was smaller then the second and normal sorting order is
		// requested, tmp will be a positive number, so we return true if it
		// actually is, false otherwise.
		return tmp > 0;
	}
	else {
		// For all other columns the default implementation will suffice.
		return Item::operator<(rhs);
	}
}


// Event handler: the listboxes send MSG_SCROLL whenever the selection changes
// and MSG_ACTIVATE whenever an item is double clicked (or single clicked if
// this option is turned on) or the enter or space keys are pressed.
void MyDialog::HandleEvent(Widget &obj, int msg, int arg1, int arg2) {
	Dialog::HandleEvent(obj, msg, arg1, arg2);

	// Big bad ugly switch that maps messages to functions.
	switch (msg) {
		case MSG_ACTIVATE: {
			// For each checkbox that was clicked enable or disable the
			// appropriate feature of the first listbox.
			if (obj == chkHeader1) {
				list1.SetHeaderVisibility(chkHeader1.Selected());
			}
			if (obj == chkSort1) {
				list1.EnableAutosort(chkSort1.Selected());
			}
			if (obj == chkResize1) {
				list1.EnableColumnResizing(chkResize1.Selected());
			}
			else if (obj == chkMulti1) {
				list1.EnableMultiSelect(chkMulti1.Selected());
			}
			else if (obj == chkSingle1) {
				list1.EnableSingleClick(chkSingle1.Selected());
			}
			else if (obj == chkGrid1) {
				list1.EnableGridLines(chkGrid1.Selected());
			}
			// When an item in list 1 is activated, display a simple
			// message box that will show that we successfully intercepted
			// the message.
			else if (obj == list1) {
				// line 1: which item was activated?
				char tmp1[256];
				usprintf(tmp1, "Item #%d of list 1 was activated:", arg1);

				// line 2: the contents of all the subitems (columns) of the activated item
				char tmp2[256];
				tmp2[0] = 0;
				// for all columns
				for (int i=0; i<list1.GetColumnCount(); i++) {
					// arg1 is the index of the activated item
					ustrcat(tmp2, list1.GetItem(arg1)->GetText(i));
					ustrcat(tmp2, " ");
				}

				// display the message box
				MessageBox msg("Woohoo!", tmp1, tmp2, NULL, "OK");
				msg.Popup(this);
			}
		}
		break;

		case MSG_SCROLL: {
			// When the selection of list 2 changes, we make sure this is reflected
			// in the editbox just below it.
			if (obj == list2) {
				char tmp[64];
				usprintf(tmp, "%d (%s)", arg1, list2.GetItem(arg1)->GetText());
				txtSelected2.SetText(tmp);
			}
		}
		break;
	}
}


int main() {
	Error err = InstallMASkinG("allegro.cfg");
	if (err) {
		err.Report();
	}

	MyDialog *dlg = new MyDialog;
	dlg->Execute();
	delete dlg;

	ExitMASkinG();
	return 0;
}
END_OF_MAIN();
