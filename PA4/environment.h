#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

class Environment {

	CgenNodeP class_;

public:
	Environment(CgenNodeP class_) { this->class_ = class_; }

};

#endif