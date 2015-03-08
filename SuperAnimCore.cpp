//
//	SuperAnimCore.cpp
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
#include <assert.h>
#include <string.h>
//#include <vector>
//#include <map>
#include "core/map.h"
#include "core/os/os.h"
//#include <algorithm>
#include "SuperAnimCommon.h"


#ifndef max
#define max(x,y) (((x) < (y)) ? (y) : (x))
#endif

namespace SuperAnim {
	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	SuperAnimMatrix3::SuperAnimMatrix3()
	{
	}
	void SuperAnimMatrix3::LoadIdentity()
	{
		m01	= m02 = m10 = m12 = m20 = m21 = 0;
		m00 = m11 = m22 = 1;
	}
	
	SuperAnimMatrix3 SuperAnimMatrix3::operator*(const SuperAnimMatrix3 &theMat) const
	{
		SuperAnimMatrix3 aResult;
		aResult.m00 = m00*theMat.m00 + m01*theMat.m10 + m02*theMat.m20;
		aResult.m01 = m00*theMat.m01 + m01*theMat.m11 + m02*theMat.m21;
		aResult.m02 = m00*theMat.m02 + m01*theMat.m12 + m02*theMat.m22;
		aResult.m10 = m10*theMat.m00 + m11*theMat.m10 + m12*theMat.m20;
		aResult.m11 = m10*theMat.m01 + m11*theMat.m11 + m12*theMat.m21;
		aResult.m12 = m10*theMat.m02 + m11*theMat.m12 + m12*theMat.m22;
		aResult.m20 = m20*theMat.m00 + m21*theMat.m10 + m22*theMat.m20;
		aResult.m21 = m20*theMat.m01 + m21*theMat.m11 + m22*theMat.m21;
		aResult.m22 = m20*theMat.m02 + m21*theMat.m12 + m22*theMat.m22;
		return aResult;
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	SuperAnimTransform::SuperAnimTransform()
	{
		mMatrix.LoadIdentity();
	}
	
	void SuperAnimTransform::Scale(float sx, float sy)
	{
		mMatrix.m00 *= sx;
		mMatrix.m01 *= sx;
		mMatrix.m02 *= sx;
		mMatrix.m10 *= sy;
		mMatrix.m11 *= sy;
		mMatrix.m12 *= sy;
	}
	
	SuperAnimTransform SuperAnimTransform::TransformSrc(const SuperAnimTransform& theSrcTransform)
	{	
		SuperAnimTransform aNewTransform;	
		
		aNewTransform.mMatrix.m00 = mMatrix.m00*theSrcTransform.mMatrix.m00 + mMatrix.m01*theSrcTransform.mMatrix.m10;
		aNewTransform.mMatrix.m01 = mMatrix.m00*theSrcTransform.mMatrix.m01 + mMatrix.m01*theSrcTransform.mMatrix.m11;
		aNewTransform.mMatrix.m10 = mMatrix.m10*theSrcTransform.mMatrix.m00 + mMatrix.m11*theSrcTransform.mMatrix.m10;
		aNewTransform.mMatrix.m11 = mMatrix.m10*theSrcTransform.mMatrix.m01 + mMatrix.m11*theSrcTransform.mMatrix.m11;
		aNewTransform.mMatrix.m02 = mMatrix.m02 + mMatrix.m00*theSrcTransform.mMatrix.m02 + mMatrix.m01*theSrcTransform.mMatrix.m12;
		aNewTransform.mMatrix.m12 = mMatrix.m12 + mMatrix.m10*theSrcTransform.mMatrix.m02 + mMatrix.m11*theSrcTransform.mMatrix.m12;
		
		return aNewTransform;
	}
	
	SuperAnimTransform SuperAnimTransform::InterpolateTo(const SuperAnimTransform& theNextTransform, float thePct)
	{
		SuperAnimTransform aNewTransform;
		
		aNewTransform.mMatrix.m00 = (mMatrix.m00 * (1.0f - thePct)) + (theNextTransform.mMatrix.m00 * thePct);
		aNewTransform.mMatrix.m01 = (mMatrix.m01 * (1.0f - thePct)) + (theNextTransform.mMatrix.m01 * thePct);
		aNewTransform.mMatrix.m10 = (mMatrix.m10 * (1.0f - thePct)) + (theNextTransform.mMatrix.m10 * thePct);
		aNewTransform.mMatrix.m11 = (mMatrix.m11 * (1.0f - thePct)) + (theNextTransform.mMatrix.m11 * thePct);
		
		aNewTransform.mMatrix.m02 = (mMatrix.m02 * (1.0f - thePct)) + (theNextTransform.mMatrix.m02 * thePct);
		aNewTransform.mMatrix.m12 = (mMatrix.m12 * (1.0f - thePct)) + (theNextTransform.mMatrix.m12 * thePct);
		
		return aNewTransform;
	}

    //////////////////////////////////////////////////////////////////////////

    Color::Color() :
    mRed(0),
    mGreen(0),
    mBlue(0),
    mAlpha(255)
    {
    }

    Color::Color(int theRed, int theGreen, int theBlue, int theAlpha) :
    mRed(theRed),
    mGreen(theGreen),
    mBlue(theBlue),
    mAlpha(theAlpha)
    {
    }

    Color Color::InterpolateTo(const Color& theNextColor, float thePct){
        return Color((int)(mRed * (1.0f - thePct) + theNextColor.mRed * thePct),
                     (int)(mGreen * (1.0f - thePct) + theNextColor.mGreen * thePct),
                     (int)(mBlue * (1.0f - thePct) + theNextColor.mBlue * thePct),
                     (int)(mAlpha * (1.0f - thePct) + theNextColor.mAlpha * thePct));
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////


    BufferReader::BufferReader()
    {
        mDataBitSize = 0;
        mReadBitPos = 0;
        mWriteBitPos = 0;
    }

    BufferReader::~BufferReader()
    {
    }

    void BufferReader::SetData(uchar* thePtr, int theCount)
    {
        mData.clear();
        Error e = mData.resize(theCount);

//            mData.insert(mData.begin(), thePtr, thePtr + theCount);

        //moved mData.insert from std::vector to this for Vector.
        uchar* myptr = mData.ptr();
        memcpy(myptr, thePtr, theCount * sizeof(uchar));

        mDataBitSize = mData.size() * 8;
    }

    uchar* BufferReader::GetDataPtr()
    {
        if (mData.size() == 0)
            return NULL;
        return &mData[0];
    }

    int BufferReader::GetDataLen() const
    {
        return (mDataBitSize + 7) / 8; // Round up
    }

    void BufferReader::Clear()
    {
        mReadBitPos = 0;
        mWriteBitPos = 0;
        mDataBitSize = 0;
        mData.clear();
    }

    uchar BufferReader::ReadByte() const
    {
        if ((mReadBitPos + 7)/8 >= (int)mData.size())
        {
            return 0; // Underflow
        }

        if (mReadBitPos % 8 == 0)
        {
            uchar b = mData[mReadBitPos/8];
            mReadBitPos += 8;
            return b;
        }
        else
        {
            int anOfs = mReadBitPos % 8;

            uchar b = 0;

            b = mData[mReadBitPos/8] >> anOfs;
            b |= mData[(mReadBitPos/8)+1] << (8 - anOfs);

            mReadBitPos += 8;

            return b;
        }
    }

    bool BufferReader::ReadBoolean() const
    {
        return ReadByte() != 0;
    }

    short BufferReader::ReadShort() const
    {
        short aShort = ReadByte();
        aShort |= ((short) ReadByte() << 8);
        return aShort;
    }

    long BufferReader::ReadLong() const
    {
        long aLong = ReadByte();
        aLong |= ((long) ReadByte()) << 8;
        aLong |= ((long) ReadByte()) << 16;
        aLong |= ((long) ReadByte()) << 24;

        return aLong;
    }

    String	BufferReader::ReadString() const
    {
        String aString;
        int aLen = ReadShort();

        for (int i = 0; i < aLen; i++)
            aString += (char) ReadByte();

        return aString;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
	
	static int sAnimObjIndex = 0;
	static bool sShouldStartAnimObjDrawItr = false;
	void BeginIterateAnimObjDrawInfo(){
		sAnimObjIndex = 0;
		sShouldStartAnimObjDrawItr = true;
	}
	
    bool IterateAnimObjDrawInfo(const SuperAnimHandler *theHandler, SuperAnimObjDrawInfo& theOutputObjDrawInfo){
//        MYPRINT("+IterateAnimObjDrawInfo\n");
		if (!sShouldStartAnimObjDrawItr) {
			assert(false && "Forgot to call BeginIterateAnimObjDrawInfo?");
			return false;
		}
		
        if (!theHandler->IsValid()) {
			assert(false && "The Animation handler is not valid.");
			return false;
		}
		
        SuperAnimMainDef *aMainDef = SuperAnimDefMgr::GetInstance()->Load_GetSuperAnimMainDef(theHandler->mMainDefKey);
		if (aMainDef == NULL) {
			assert(false && "I can't find the Animation definition.");
			return false;
		}
//        MYPRINT("Key: %s, frames: %d\n", theHandler->mMainDefKey.utf8().get_data(), aMainDef->mFrames.size());
		
        int aCurFrameNum = (int)theHandler->mCurFrameNum;
//        MYPRINT("CurrentFrame: %d\n", aCurFrameNum);

		SuperAnimFrame *aCurFrame = &aMainDef->mFrames[aCurFrameNum];
		if (sAnimObjIndex >= aCurFrame->mObjectVector.size()) {
			// we have iterated all objects in this frame
			sShouldStartAnimObjDrawItr = false;
			return false;
		}
		
		SuperAnimObject *aCurObject = &aCurFrame->mObjectVector[sAnimObjIndex];
//        MYPRINT("ObjIndex: %d, ObjNum: %d ResNum: %d\n", sAnimObjIndex, aCurObject->mObjectNum, aCurObject->mResNum);

		// find the image, fill the sprite id
		SuperAnimImage *aSuperAnimImage = &aMainDef->mImageVector[aCurObject->mResNum];
		theOutputObjDrawInfo.mSpriteId = aSuperAnimImage->mSpriteId;
//        MYPRINT("%s : %p", aSuperAnimImage->mImageName.utf8().get_data(), aSuperAnimImage->mSpriteId);
		
		// do the interpolateion to next frame for transform & color
		if (aCurFrameNum == aMainDef->mEndFrameNum) {
			// reach the end frame, don't need to do any interpolation
			theOutputObjDrawInfo.mTransform = aCurObject->mTransform;
			theOutputObjDrawInfo.mColor = aCurObject->mColor;
		} else {
			int aNextFrameNum = aCurFrameNum + 1;
			bool finishedInterp = false;
			SuperAnimFrame *aNextFrame = &aMainDef->mFrames[aNextFrameNum];
			for (int i = 0; i < aNextFrame->mObjectVector.size(); ++i) {
				SuperAnimObject *anObj = &aNextFrame->mObjectVector[i];
				if (anObj->mObjectNum == aCurObject->mObjectNum &&
					anObj->mResNum == aCurObject->mResNum) {
                    float anInterp = theHandler->mCurFrameNum - aCurFrameNum;
					theOutputObjDrawInfo.mTransform = aCurObject->mTransform.InterpolateTo(anObj->mTransform, anInterp);
					theOutputObjDrawInfo.mColor = aCurObject->mColor.InterpolateTo(anObj->mColor, anInterp);
					finishedInterp = true;
					break;
				}
			}
			if (!finishedInterp) {
				// we miss the object in next frame?
				// never mind
				theOutputObjDrawInfo.mTransform = aCurObject->mTransform;
				theOutputObjDrawInfo.mColor = aCurObject->mColor;
			}
		}
		
		theOutputObjDrawInfo.mTransform = theOutputObjDrawInfo.mTransform.TransformSrc(aSuperAnimImage->mTransform);
		SuperAnimMatrix3 aMatrix;
		aMatrix.LoadIdentity();
		aMatrix.m02 = aSuperAnimImage->mWidth * 0.5f;
		aMatrix.m12 = aSuperAnimImage->mHeight * 0.5f;
		theOutputObjDrawInfo.mTransform.mMatrix = theOutputObjDrawInfo.mTransform.mMatrix * aMatrix;
		
		sAnimObjIndex++;
//        MYPRINT("-IterateAnimObjDrawInfo\n");
		return true;
	}
	
    void IncAnimFrameNum(SuperAnimHandler *theMainDefHandler, float theDeltaTime, bool &hitNewLabel){
		
        if (!theMainDefHandler->IsValid()) {
			return;
		}
		
        int aLastFrameNum = (int)theMainDefHandler->mCurFrameNum;
        theMainDefHandler->mCurFrameNum += theDeltaTime * theMainDefHandler->mAnimRate;
        int aCurFrame = (int)theMainDefHandler->mCurFrameNum;
		
		if (aCurFrame != aLastFrameNum) // Reach new frame
		{
			// Check whether reach a new label frame
			bool aIsNewLabel = false;
            if (aCurFrame >= theMainDefHandler->mLastFrameNumOfCurLabel) {
                theMainDefHandler->mCurFrameNum = theMainDefHandler->mLastFrameNumOfCurLabel;
				aIsNewLabel = true;
			}
			
			hitNewLabel = aIsNewLabel;
		}
	}
	
    bool HasSection(const SuperAnimHandler &theHandler, String theLabelName){
        print_line("+HasSection(" + theHandler.mMainDefKey + ", " + theLabelName + ")");

		SuperAnimMainDef *aMainDef = SuperAnimDefMgr::GetInstance()->Load_GetSuperAnimMainDef(theHandler.mMainDefKey);
		if (aMainDef == NULL) {
            print_line("-HasSection() NoMainDef Found");
			return false;
		}

        for(int i = 0; i <  aMainDef->mLabels.size(); i++){
            if(theLabelName == aMainDef->mLabels[i].mLabelName){
                print_line("-HasSection() Label Found");
                return true;
            }
        }
//		for (SuperAnimLabelArray::const_iterator it = aMainDef->mLabels.begin(); it != aMainDef->mLabels.end(); ++it)
//		{
//			if (it->mLabelName == theLabelName)
//				return true;
//		}
		
		return false;
	}
	
    bool PlayBySection(SuperAnimHandler &theHandler, String theLabelName){
        MYPRINT("+PlayBySection\n");
		SuperAnimMainDef *aMainDef = SuperAnimDefMgr::GetInstance()->Load_GetSuperAnimMainDef(theHandler.mMainDefKey);        
		if (aMainDef == NULL) {
            OS::get_singleton()->print("MainDef: %p", aMainDef);
			theHandler.mIsHandlerValid = false;
			return false;
		}


        for(int i = 0; i < aMainDef->mLabels.size(); i++){
            if(aMainDef->mLabels[i].mLabelName == theLabelName){
                SuperAnimLabel it = aMainDef->mLabels[i];
                theHandler.mCurFrameNum = it.mStartFrameNum;
                theHandler.mCurLabel = theLabelName;
                theHandler.mFirstFrameNumOfCurLabel = it.mStartFrameNum;
                theHandler.mLastFrameNumOfCurLabel = it.mEndFrameNum;
                theHandler.mIsHandlerValid = true;
                MYPRINT("-PlayBySection Success\n");
                return true;
            }
        }
//		for (SuperAnimLabelArray::const_iterator it = aMainDef->mLabels.begin(); it != aMainDef->mLabels.end(); ++it)
//		{
//			if (it->mLabelName == theLabelName)
//			{
//				theHandler.mCurFrameNum = it->mStartFrameNum;
//				theHandler.mCurLabel = theLabelName;
//				theHandler.mFirstFrameNumOfCurLabel = it->mStartFrameNum;
//				theHandler.mLastFrameNumOfCurLabel = it->mEndFrameNum;
//				theHandler.mIsHandlerValid = true;
//				return true;
//			}
//		}
        MYPRINT("-PlayBySection Failed\n");
		theHandler.mIsHandlerValid = false;
		return false;
	}
	
    bool LoadAnimFile(const String &theAbsAnimFile){
		return SuperAnimDefMgr::GetInstance()->Load_GetSuperAnimMainDef(theAbsAnimFile) != NULL;
	}
	
    void UnloadAnimFile(const String &theAbsAnimFile){
		SuperAnimDefMgr::GetInstance()->UnloadSuperAnimMainDef(theAbsAnimFile);
	}
	

    //Super AnimDefManager we are getting rid of it.
	static SuperAnimDefMgr *sInstance = NULL;
	
	SuperAnimDefMgr::SuperAnimDefMgr()
	{
		
	}
	
	SuperAnimDefMgr::~SuperAnimDefMgr()
	{
		mMainDefCache.clear();
	}
	
	SuperAnimDefMgr * SuperAnimDefMgr::GetInstance()
	{
		if (sInstance == NULL)
		{
			sInstance = new SuperAnimDefMgr();
		}
		
		return sInstance;
	}
	
	void SuperAnimDefMgr::DestroyInstance()
	{
		if (sInstance)
		{
			delete sInstance;
			sInstance = NULL;
		}
	}
		
    void SuperAnimDefMgr::addMainDef(const String &theSuperAnimFile, SuperAnimMainDef* ptr)
    {
        MYPRINT("+SupserAnimDefMgr::addMainDef\n");
        OS::get_singleton()->print("MainDef Added: %s %p\n", theSuperAnimFile.utf8().get_data(), ptr);
        mMainDefCache[theSuperAnimFile] = ptr;
        MYPRINT("-SupserAnimDefMgr::addMainDef\n");
    }

    bool SuperAnimDefMgr::LoadSuperAnimMainDef(const String &theSuperAnimFile)
    {
        return false;
	}
	
    SuperAnimMainDef *SuperAnimDefMgr::Load_GetSuperAnimMainDef(const String &theSuperAnimFile)
	{
//        MYPRINT("+SuperAnimDefMgr::Load_GetSuperAnimMainDef(%s)\n", theSuperAnimFile.utf8().get_data());
        SuperAnimMainDefMap::Element *anItr = mMainDefCache.find(theSuperAnimFile);
        if (anItr != NULL)
        {
//            MYPRINT("-SuperAnimDefMgr::Load_GetSuperAnimMainDef() %p\n", anItr->value());
            return anItr->value();
        }
		
//        if (LoadSuperAnimMainDef(theSuperAnimFile) == false)
//            return NULL;
		
//        return Load_GetSuperAnimMainDef(theSuperAnimFile);
        return NULL;
	}
	
    void SuperAnimDefMgr::UnloadSuperAnimMainDef(const String &theName)
	{
//		SuperAnimMainDefMap::iterator anItr = mMainDefCache.find(theName);
//		if (anItr == mMainDefCache.end())
//			return;
		
//		// unload the sprites
//		for (int i = 0; i < (int)anItr->second.mImageVector.size(); ++i)
//		{
//			SuperAnimImage anImage = anItr->second.mImageVector[i];
//			UnloadSuperSprite(anImage.mSpriteId);
//		}
		
//		mMainDefCache.erase(anItr);        
	}	
}// end namespace
