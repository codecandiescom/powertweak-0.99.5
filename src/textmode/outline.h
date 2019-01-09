/*
 * outline.h
 *
 * Turbo Vision - Version 2.0
 *
 * Copyright (c) 1994 by Borland International
 * All Rights Reserved.
 *
 * Modified by Sergio Sigala <ssigala@globalnet.it>
 */

#if defined( Uses_TOutlineViewer ) && !defined( __TOutlineViewer )
#define __TOutlineViewer

const int
  ovExpanded = 0x01,
  ovChildren = 0x02,
  ovLast     = 0x04;

const int
  cmOutlineItemSelected = 301;

/* ------------------------------------------------------------------------*/
/*      class TOutlineViewer                                               */
/*                                                                         */
/*      Palette layout                                                     */
/*        1 = Normal color                                                 */
/*        2 = Focus color                                                  */
/*        3 = Select color                                                 */
/*        4 = Not expanded color                                           */
/* ------------------------------------------------------------------------*/

class TRect;
class TScrollBar;
class TEvent;

class TOutlineViewer : public TScroller
{
public:
    TOutlineViewer(const TRect& bounds, TScrollBar* aHScrollBar,
        TScrollBar* aVScrollBar);
    TOutlineViewer(StreamableInit s);
    virtual void adjust(void* node, Boolean expand)=0;
    virtual void draw();
    virtual void focused(int i);
    virtual void* getChild(void* node, int i)=0;
    virtual char* getGraph(int level, long lines, ushort flags);
    virtual int getNumChildren(void* node)=0;
    virtual void* getNode(int i);
    virtual TPalette& getPalette() const;
    virtual void* getRoot()=0;
    virtual char* getText(void* node)=0;
    virtual void handleEvent(TEvent& event);
    virtual Boolean hasChildren(void* node)=0;
    virtual Boolean isExpanded(void* node)=0;
    virtual Boolean isSelected(int i);
    virtual void selected(int i);
    virtual void setState(ushort aState, Boolean enable);

    void update();
    void expandAll(void* node);
    void* firstThat(Boolean (*test)(TOutlineViewer*, void*,int,int,long,
        ushort));
    void* forEach(Boolean (*action)(TOutlineViewer*, void*,int,int,long,
        ushort));

    char* createGraph(int level, long lines, ushort flags, int levWidth,
        int endWidth, const char* chars);

    int foc;
    static const char* graphChars;

protected:
    static void disposeNode(void* node);
#ifndef NO_STREAMABLE    
    virtual void write( opstream& os );
    virtual void *read( ipstream& is );

public:
    static TStreamable *build();
#endif
public:    
    static const char * const name;

private:
    void adjustFocus(int newFocus);
    void* iterate(Boolean (*action)(TOutlineViewer*, void*, int, int, long,
                ushort), Boolean checkResult);
};

#ifndef NO_STREAMABLE

inline TOutlineViewer::TOutlineViewer( StreamableInit s) :
    TScroller(s)
{
}

#endif

#endif // Uses_TOutlineViewer

#if defined( Uses_TOutline ) && !defined( __TOutline )
#define __TOutline

/* ------------------------------------------------------------------------*/
/*      class TOutline                                                     */
/*                                                                         */
/*      Palette layout                                                     */
/*        1 = Normal color                                                 */
/*        2 = Focus color                                                  */
/*        3 = Select color                                                 */
/*        4 = Not expanded color                                           */
/* ------------------------------------------------------------------------*/

class TRect;
class TScrollBar;
class TEvent;

class TOutline : public TOutlineViewer
{
public:
    TOutline(const TRect& bounds, TScrollBar* aHScrollBar, TScrollBar* aVScrollBar,
        void* aRoot);
    ~TOutline();

    virtual void adjust(void* node, Boolean expand);
    virtual void* getRoot();
    virtual int getNumChildren(void* node);
    virtual void* getChild(void* node, int i);
    virtual char* getText(void* node);
    virtual Boolean isExpanded(void* node);
    virtual Boolean hasChildren(void* node);
    virtual void selected(int i);

    void* root;

protected:
#ifndef NO_STREAMABLE
    virtual void write( opstream& os );
    virtual void* read( ipstream& is );
    virtual void writeNode( void*, opstream& );
    virtual void* readNode( ipstream& );
    TOutline( StreamableInit );

public:
    static TStreamable* build();
#endif    
public:
    static const char* const name;

private:
    virtual const char *streamableName() const
        { return name; }

};

#ifndef NO_STREAMABLE
inline TOutline::TOutline( StreamableInit s ) : TOutlineViewer( s )
{
}

inline ipstream& operator >> ( ipstream& is, TOutline& o )
    { return is >> (TStreamable&)o; }
inline ipstream& operator >> ( ipstream& is, TOutline*& o )
    { return is >> (void *&)o; }

inline opstream& operator << ( opstream& os, TOutline& o )
    { return os << (TStreamable&)o; }
inline opstream& operator << ( opstream& os, TOutline* o )
    { return os << (TStreamable*)o; }

#endif

#endif // Uses_TOutline
