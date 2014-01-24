#ifndef WIDDATA_H
#define WIDDATA_H

#define IMPORT_WIDTHS extern struct STRUCT_TAG(fwidths) fwidths
#define STRUCT_TAG(x) x##_tag
#define SETFIELDWID(name,field,newval) (MSGFIELD(name,field) = newval) 
#define MSGFIELD(name,field) fwidths.MSGWID(name).FIELD(field)
#define MSGWID(name) name##_wid
#define FIELD(field) field##_wid


struct STRUCT_TAG(fwidths)
{
	struct 
	{
		unsigned int FIELD(keysym);
		unsigned int FIELD(state);
	} MSGWID( MsgKey );
	struct
	{
		unsigned int FIELD(x);
		unsigned int FIELD(y);
	} MSGWID( MsgPointerSet );
	struct
	{
		unsigned int FIELD(dx);
		unsigned int FIELD(dy);
	} MSGWID( MsgPointerMove );
	struct
	{
		unsigned int FIELD(button);
		unsigned int FIELD(state);
	} MSGWID( MsgPointer );
	struct
	{
		unsigned int FIELD(width);
		unsigned int FIELD(height);
	} MSGWID( MsgSetResolution );
	struct
	{
		unsigned int FIELD( monNum );
	} MSGWID( MsgDisplayNum );

		
};

#endif

