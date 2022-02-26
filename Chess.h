// Lotfi Achiri 
// Master 2 techmed 
// 2021/2022


/*
* This observer class runs constantly after the run to recuperate the new 
*/

class Observer : public vtkCommand {
public:
	static Observer* New()
	{
		return new Observer;
	}
	
	void Execute(vtkObject*, unsigned long, void*);
	
	void setActorSource(vtkActor *actor)
	{
		this->Actor = actor;
		//actor->SetOrigin(0.8 , -13 , 0);
		Center = actor->GetOrigin();
		Y = Center[2];
		X = Center[1];
	}

	void SetYMin(int res) { this->YMin = res; }
	void SetYMax(int res) { this->YMax = res; }
	void SetXMin(int res) { this->XMin = res; }
	void SetXMax(int res) { this->XMax = res; }

private:
	vtkActor *Actor;
	int  YMin, YMax, XMin, XMax;
	double * Center;
	float Y, X;
	bool up;
};