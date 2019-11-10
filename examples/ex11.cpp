// example 11: Shows how to make user defined tooltip help bubbles.

#include "../include/MASkinG.h"
using namespace MAS;


// Our user defined tooltip class which will draw geometric shapes instead of text.
// But all tooltips work with tooltip text, so we will have to parse the text we
// receive and if it contains special instructions for drawing shapes, we'll draw
// shapes otherwise we'll call the default tooltip implementation.
class MyTooltip : public Tooltip {
	protected:
		void Draw(Bitmap &canvas) {
			// Parse the tooltip text and if it starts with an instruction for drawing
			// a geometric shape, then draw the shape. In this example the string
			// "@shape=" means that a shape whould be drawn but you could think of
			// anything you want in a real application.
			if ((ustrlen(text) >= 7) && (ustrncmp(text, "@shape=", 7) == 0)) {
				Color c1 = Color::black;
				Color c2 = Color(192,255,255);
				Color c3 = Color(255,192,255);

				// Draw the bubble
				canvas.Clear(c2);
				canvas.Rectangle(0, 0, w()-1, h()-1, c1);
				
				// Draw the shape. This class supports three different shapes: circles,
				// squares and triangles
				switch (atoi(text+7)) {
					// circle
					case 0:
						canvas.Circlefill(w()/2, h()/2, w()/4, c3);
						canvas.Circle(w()/2, h()/2, w()/4, c1);
					break;
					
					// square
					case 1: {
						Rect r = Rect(w()/4, h()/4, w()/2, h()/2);
						canvas.Rectfill(r, c3);
						canvas.Rectangle(r, c1);
					}
					break;
					
					// triangle
					case 2: {
						Point p1 = Point(w()/2, h()/4);
						Point p2 = Point(w()/6, 3*h()/4);
						Point p3 = Point(5*w()/6, 3*h()/4);
						canvas.Triangle(p1, p2, p3, c3);
						canvas.Line(p1, p2, c1);
						canvas.Line(p1, p3, c1);
						canvas.Line(p2, p3, c1);
					}
					break;
				};
			}
			else {
				// The text didn't have an instruction to draw a shape so we'll call the
				// default implementation to draw the default help bubble.
				Tooltip::Draw(canvas);
			}
		}

		// Caluclate and set the size of the bubble. If the tooltip text is an instruction
		// to draw a shape, set the size to a predefined size of 41x41, otherwise let the
		// default tooltip class take care of that.
		void CalculateSize() {
			if ((ustrlen(text) >= 7) && (ustrncmp(text, "@shape=", 7) == 0)) {
				Resize(41, 41);
			}
			else {
				Tooltip::CalculateSize();
			}
		}
};


class MyDialog : public Dialog {
	protected:
		ClearScreen desktop;
		PanelGroove panel;
		Label info;
		Label lInstructions, lCircle, lSquare, lTriangle, lText;
		MyTooltip myTooltip;

	public:
		MyDialog() : Dialog() {
			// setup and add all the widgets to the dialog
			Add(desktop);
			info.Shape(20, 20, 260, 400);
			info.SetWordWrap(true);
			info.SetText("Example 11: Shows how to derive user defined tooltip help bubbles from the default tooltip class. In this example the tooltips parse the tooltip text and if the text is in a special format they draw according to the instructions in that text, otherwise they draw a normal tooltip bubble.");
			Add(info);
			
			lInstructions.SetWordWrap(true);
			lInstructions.Setup(310, 20, 220, 40, 0, 0, "Hover the mouse over the labels below to show the toolips:", 0);
			
			panel.Shape(290, 60, 260, 70);
			lCircle.ClearFlag(D_AUTOSIZE);
			lCircle.Setup(300, 70, 80, 20, 0, 0, "Circle", 2);
			lSquare.ClearFlag(D_AUTOSIZE);
			lSquare.Setup(380, 70, 80, 20, 0, 0, "Square", 2);
			lTriangle.ClearFlag(D_AUTOSIZE);
			lTriangle.Setup(460, 70, 80, 20, 0, 0, "Triangle", 2);
			lText.ClearFlag(D_AUTOSIZE);
			lText.Setup(300, 100, 240, 20, 0, 0, "This label has normal tooltips", 2);
			
			// set the tooltip text for the labels in the groove panel: the first three have
			// special instructions to draw geometric shapes, the last one has normal text
			lCircle.SetTooltipText("@shape=0");
			lSquare.SetTooltipText("@shape=1");
			lTriangle.SetTooltipText("@shape=2");
			lText.SetTooltipText("This is a normal tooltip.");
			
			Add(lInstructions);
			Add(panel);
			Add(lCircle);
			Add(lSquare);
			Add(lTriangle);
			Add(lText);
			
			// important: register an instance of our special tooltip class with the dialog
			// so it will be used instead of the default implementation
			SetTooltipObject(&myTooltip);
		}
};

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
