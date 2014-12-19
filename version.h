#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{

	//Date Version Types
	static const char DATE[] = "21";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2010";
	static const double UBUNTU_VERSION_STYLE = 10.06;

	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";

	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 3;
	static const long BUILD = 12;
	static const long REVISION = 5;

	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 9;
	#define RC_FILEVERSION 0,3,12,5
	#define RC_FILEVERSION_STRING "0, 3, 12, 5\0"
	static const char FULLVERSION_STRING[] = "0.3.12.5";

	//SVN Version
	static const char SVN_REVISION[] = "18";
	static const char SVN_DATE[] = "2007-11-15T15:42:40.280543Z";


}
#endif //VERSION_h
