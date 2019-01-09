/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H�hne to be used for RHIDE.

 *
 *
 */

/* ---------------------------------------------------------------------- */
/*      class TInputLine                                                  */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Passive                                                     */
/*        2 = Active                                                      */
/*        3 = Selected                                                    */
/*        4 = Arrows                                                      */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TInputLine ) && !defined( __TInputLine )
#define __TInputLine

class TRect;
class TEvent;
class TValidator;

class TInputLine : public TView
{

public:

    TInputLine( const TRect& bounds, int aMaxLen );
    ~TInputLine();

    virtual uint32 dataSize();
    virtual void draw();
    virtual void getData( void *rec );
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    void selectAll( Boolean enable );
    virtual void setData( void *rec );
    virtual void setState( ushort aState, Boolean enable );
    void SetValidator(TValidator *);
    virtual Boolean valid(ushort);

    char* data;
    int maxLen;
    int curPos;
    int firstPos;
    int selStart;
    int selEnd;

    static char rightArrow;
    static char leftArrow;
    static char orightArrow;
    static char oleftArrow;

protected:
    virtual void resizeData() {}
    TValidator * validator;
    void deleteSelect();
private:

    Boolean canScroll( int delta );
    int mouseDelta( TEvent& event );
    int mousePos( TEvent& event );

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

protected:

    TInputLine( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TInputLine& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TInputLine*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TInputLine& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TInputLine* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TInputLine

