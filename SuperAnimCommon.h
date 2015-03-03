//
//	SuperAnimCommon.h
//
//  Created by Raymond Lu(Raymondlu1105@qq.com)
//  
//  All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//
#ifndef SuperAnimationMath_h
#define SuperAnimationMath_h

#include <math.h>
#include <string>
#include "core/resource.h"
#include "core/ustring.h"

#define SAM_VERSION 1
#define TWIPS_PER_PIXEL (20.0f)
#define LONG_TO_FLOAT (65536.0f)

#define FRAMEFLAGS_REMOVES		0x01
#define FRAMEFLAGS_ADDS			0x02
#define FRAMEFLAGS_MOVES		0x04
#define FRAMEFLAGS_FRAME_NAME	0x08

#define MOVEFLAGS_ROTATE		0x4000
#define MOVEFLAGS_COLOR			0x2000
#define MOVEFLAGS_MATRIX		0x1000
#define MOVEFLAGS_LONGCOORDS	0x0800

namespace SuperAnim{

	class SuperAnimMatrix3
	{
	public:
		union
		{
			float m[3][3];
			struct
			{
				float m00, m01, m02;
				float m10, m11, m12;
				float m20, m21, m22;
			};
		};

	public:
		SuperAnimMatrix3();
		void LoadIdentity();
		SuperAnimMatrix3 operator*(const SuperAnimMatrix3 &theMat) const;
	};
	//////////////////////////////////////////////////////////////////////////
	class SuperAnimTransform
	{
	public:
		SuperAnimMatrix3				mMatrix;

	public:
		SuperAnimTransform();
		void Scale(float sx, float sy);
		SuperAnimTransform				TransformSrc(const SuperAnimTransform& theSrcTransform);		
		SuperAnimTransform				InterpolateTo(const SuperAnimTransform& theNextTransform, float thePct);
	};

	// for sprite
	typedef void* SuperAnimSpriteId;
	#define InvalidSuperAnimSpriteId NULL
	
	// implemented by super animation node
    extern SuperAnimSpriteId LoadSuperAnimSprite(String theSpriteName);
	extern void UnloadSuperSprite(SuperAnimSpriteId theSpriteId);
	
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    class Color
    {
    public:
        int mRed;
        int mGreen;
        int mBlue;
        int mAlpha;
    public:
        Color();
        Color(int theRed, int theGreen, int theBlue, int theAlpha);
        Color InterpolateTo(const Color& theNextColor, float thePct);
    };
    //////////////////////////////////////////////////////////////////////////

	// for animation main definition
    class SuperAnimHandler{
        public:
        String mMainDefKey;
        String mCurLabel;
		int mFirstFrameNumOfCurLabel;
		int mLastFrameNumOfCurLabel;
		float mCurFrameNum;
		float mAnimRate;
		float mWidth;
		float mHeight;
		bool mIsHandlerValid;
		
		SuperAnimHandler(){
			mFirstFrameNumOfCurLabel = 0;
			mLastFrameNumOfCurLabel = 0;
			mCurFrameNum = 0.0f;
			mAnimRate = 0.0f;
			mWidth = 0.0f;
			mHeight = 0.0f;
			mIsHandlerValid = false;
		}
		
		bool IsValid() const{
			return mIsHandlerValid;
		}
	};
	
	struct SuperAnimObjDrawInfo{
		SuperAnimSpriteId mSpriteId;
		SuperAnimTransform mTransform;
		Color mColor;
		SuperAnimObjDrawInfo(){
			mSpriteId = InvalidSuperAnimSpriteId;
		}
	};


    // Animation object definition

    class SuperAnimObject
    {
    public:
        int mObjectNum;
        int mResNum;
        SuperAnimTransform mTransform;
        Color mColor;
    };

    #include "core/vector.h"
    #include "core/map.h"

    typedef Vector<SuperAnimObject> SuperAnimObjectVector;
    typedef Map<int, SuperAnimObject> IntToSuperAnimObjectMap;

    class SuperAnimImage
    {
    public:
        SuperAnimSpriteId mSpriteId;
        String mImageName;
        int mWidth;
        int mHeight;
        SuperAnimTransform mTransform;
    };
    typedef Vector<SuperAnimImage> SuperAnimImageVector;

    class SuperAnimFrame
    {
    public:
        SuperAnimObjectVector mObjectVector;
    };
    typedef Vector<SuperAnimFrame> SuperAnimFrameVector;
    typedef Map<String, int> StringToIntMap;
    class SuperAnimLabel{
    public:
        String mLabelName;
        int mStartFrameNum;
        int mEndFrameNum;

        bool operator < (const SuperAnimLabel &b)  const{
            print_line("Operator < called" + itos(mStartFrameNum) + " " + itos(b.mStartFrameNum));
            if (mStartFrameNum != b.mStartFrameNum) {
                return mStartFrameNum < b.mStartFrameNum;
            }
            return true;
        }
    };
    typedef Vector<SuperAnimLabel> SuperAnimLabelArray;

    class SuperAnimMainDef
    {
    public:
        SuperAnimFrameVector mFrames;
        int mStartFrameNum;
        int mEndFrameNum;
        int mAnimRate;
        SuperAnimLabelArray mLabels;
        int mX;
        int mY;
        int mWidth;
        int mHeight;
        SuperAnimImageVector mImageVector;
    };
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    typedef unsigned char uchar;
    typedef Vector<uchar> ByteVector;

    class BufferReader
    {
    public:
        ByteVector				mData;
        int						mDataBitSize;
        mutable int				mReadBitPos;
        mutable int				mWriteBitPos;

    public:
        BufferReader();
        virtual ~BufferReader();
        void					SetData(uchar* thePtr, int theCount);
        uchar*					GetDataPtr();
        int						GetDataLen() const;
        void					Clear();

        uchar					ReadByte() const;
        bool					ReadBoolean() const;
        short					ReadShort() const;
        long					ReadLong() const;
        String				ReadString() const;
    };


    //TODO: remove this class
    //We wont' need the SuperAnimDefMgr because the Res returned when we load a sam is the SuperAnimMainDef.
    //This is the definition of the file.
    typedef Map<String, SuperAnimMainDef*> SuperAnimMainDefMap;
    class SuperAnimDefMgr
    {
    private:
        SuperAnimMainDefMap mMainDefCache;
    private:
        SuperAnimDefMgr();
        ~SuperAnimDefMgr();

        // String theSuperAnimFile include the absolute path
        bool LoadSuperAnimMainDef(const String &theSuperAnimFile);
    public:
        static SuperAnimDefMgr *GetInstance();
        static void DestroyInstance();

        void addMainDef(const String &theSuperAnimFile, SuperAnimMainDef*);

        // String theSuperAnimFile include the absolute path
        SuperAnimMainDef *Load_GetSuperAnimMainDef(const String &theSuperAnimFile);
        void UnloadSuperAnimMainDef(const String &theName);
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
	
	// implemented by super animation core
    extern SuperAnimHandler GetSuperAnimHandler(const String &theAbsAnimFile);
	extern void BeginIterateAnimObjDrawInfo();
	extern bool IterateAnimObjDrawInfo(const SuperAnimHandler &theHandler, SuperAnimObjDrawInfo& theOutputObjDrawInfo);
	extern void IncAnimFrameNum(SuperAnimHandler &theMainDefHandler, float theDeltaTime, bool &hitNewFrame);
    extern bool HasSection(const SuperAnimHandler &theHandler, String theLabelName);
    extern bool PlayBySection(SuperAnimHandler &theHandler, String theLabelName);
    extern bool LoadAnimFile(const String &theAbsAnimFile);
    extern void UnloadAnimFile(const String &theAbsAnimFile);
};

// platform related functions
extern unsigned char* GetFileData(String pszFileName, const char* pszMode, unsigned long * pSize);
#endif
