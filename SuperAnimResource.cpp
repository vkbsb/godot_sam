//
//  SuperAnimData.cpp
//  GodotEditing
//
//  Created by vamsi krishna on 24/02/15.
//  Copyright (c) 2015 vamsi krishna. All rights reserved.
//

#include "core/io/file_access_buffered.h"
//#include "SuperAnimCommon.h"
#include "SuperAnimResource.h"
#include <stdio.h>
#include <string>

unsigned char* GetFileData(String pszFileName, const char* pszMode, unsigned long * pSize)
{
    print_line("+GetFileData");
    FileAccess *fa = FileAccess::open(pszFileName, FileAccess::READ);
    if(fa){
        size_t fsize = fa->get_len();
        uint8_t *pData = memnew_arr(uint8_t, fsize);

        fa->get_buffer(pData, fsize);

        fa->close();
        memdelete(fa);

        print_line("FileDataSize: " + itos(fsize));

        *pSize = (unsigned long)fsize;
        return (unsigned char*)pData;
    }
    print_line("-GetFileData: Error!");
    return NULL;
}

bool SuperAnimData::LoadSuperAnimMainDef(const String &theSuperAnimFile)
{
    String aFullPath = theSuperAnimFile;

    String aCurDir = "";
    int aLastSlash = MAX((int) theSuperAnimFile.rfind("\\"), (int) theSuperAnimFile.rfind("/"));
    if (aLastSlash != String::npos){
        aCurDir = theSuperAnimFile.substr(0, aLastSlash);
    }

    print_line(aCurDir.c_str());

    unsigned long aFileSize = 0;
    unsigned char *aFileBuffer = GetFileData(aFullPath, "rb", &aFileSize);
    if (aFileBuffer == NULL)
    {
        //            assert(false && "Cannot allocate memory.");
        print_line("Cannot allocate memory.");
        return false;
    }

    SuperAnim::BufferReader aBuffer;
    aBuffer.SetData(aFileBuffer, aFileSize);

    // free memory
    //        delete[] aFileBuffer;
    memdelete_arr(aFileBuffer);

    /*-------------------- test for file format reading --------------*/

    //        FileAccess *fa = FileAccess::open(aFullPath, FileAccess::READ);
    //        if(fa != NULL){
    //            if(fa->get_32() != 0x2E53414D){
    //                print_line("Bad Frmat from Test");
    //            }else{
    //                print_line("Found the format");
    //            }
    //            fa->close();
    //            memdelete(fa);
    //        }
    /*---------------------------------------------------------------*/

    if (aBuffer.ReadLong() != 0x2E53414D)
    {
        //            assert(false && "Bad file format.");
        print_line("Bad file format.");
        return false;
    }

    int aVersion = aBuffer.ReadLong();

    if (aVersion != SAM_VERSION)
    {
        //            assert(false && "Wrong version.");
        print_line("Wrong version.");
        return false;
    }

    SuperAnimMainDef &aMainDef(*this);//mMainDefCache[theSuperAnimFile];
    //        SuperAnimMainDef &aMainDef = mMainDefCache[theSuperAnimFile];

    aMainDef.mAnimRate = aBuffer.ReadByte();
    aMainDef.mX = aBuffer.ReadLong() / TWIPS_PER_PIXEL;
    aMainDef.mY = aBuffer.ReadLong() / TWIPS_PER_PIXEL;
    aMainDef.mWidth = aBuffer.ReadLong() / TWIPS_PER_PIXEL;
    aMainDef.mHeight = aBuffer.ReadLong() / TWIPS_PER_PIXEL;

    SuperAnim::SuperAnimLabelArray aSuperAnimLabelArray;

    int aNumImages = aBuffer.ReadShort();
    aMainDef.mImageVector.resize(aNumImages);
    for (int anImageNum = 0; anImageNum < aNumImages; ++anImageNum)
    {
        SuperAnim::SuperAnimImage &aSuperAnimImage = aMainDef.mImageVector[anImageNum];
        aSuperAnimImage.mImageName = aBuffer.ReadString();
        aSuperAnimImage.mWidth = aBuffer.ReadShort();
        aSuperAnimImage.mHeight = aBuffer.ReadShort();

        aSuperAnimImage.mTransform.mMatrix.m00 = aBuffer.ReadLong() / (LONG_TO_FLOAT * TWIPS_PER_PIXEL);
        aSuperAnimImage.mTransform.mMatrix.m01 = -aBuffer.ReadLong() / (LONG_TO_FLOAT * TWIPS_PER_PIXEL);
        aSuperAnimImage.mTransform.mMatrix.m10 = -aBuffer.ReadLong() / (LONG_TO_FLOAT * TWIPS_PER_PIXEL);
        aSuperAnimImage.mTransform.mMatrix.m11 = aBuffer.ReadLong() / (LONG_TO_FLOAT * TWIPS_PER_PIXEL);
        aSuperAnimImage.mTransform.mMatrix.m02 = aBuffer.ReadShort() / TWIPS_PER_PIXEL;
        aSuperAnimImage.mTransform.mMatrix.m12 = aBuffer.ReadShort() / TWIPS_PER_PIXEL;

        String aImagePath;
        if (aCurDir.empty()) {
            aImagePath = aSuperAnimImage.mImageName;
        } else {
            aImagePath = aCurDir + "/" + aSuperAnimImage.mImageName;
        }
        print_line("load image: " + aImagePath);
        //TODO: get the sprite id here.
        //get the texture resource and add it to the SuperAnimSprite stuff and the aSuperAnimImageId.
        aSuperAnimImage.mSpriteId = SuperAnim::LoadSuperAnimSprite(aImagePath);
    }

    int aNumFrames = aBuffer.ReadShort();
    //        assert(aNumFrames > 0 && "We don't have valid frames.");
    if(aNumFrames <= 0){
        print_line("We don't have valid frames.");
        return false;
    }
    aMainDef.mStartFrameNum = 0;
    aMainDef.mEndFrameNum = aNumFrames - 1;
    aMainDef.mFrames.resize(aNumFrames);
    SuperAnim::IntToSuperAnimObjectMap aCurObjectMap;
    print_line("NumFrames: " + itos(aNumFrames));
    for (int aFrameNum = 0; aFrameNum < aNumFrames; ++aFrameNum)
    {
        SuperAnim::SuperAnimFrame &aFrame = aMainDef.mFrames[aFrameNum];
        SuperAnim::uchar aFrameFlags = aBuffer.ReadByte();

        if (aFrameFlags & FRAMEFLAGS_REMOVES)
        {
            int aNumRemoves = aBuffer.ReadByte();
            for (int aRemoveNum = 0; aRemoveNum < aNumRemoves; ++ aRemoveNum)
            {
                int anObjectId = aBuffer.ReadShort();
                SuperAnim::IntToSuperAnimObjectMap::Element *pElement = aCurObjectMap.find(anObjectId);
                if(pElement != NULL){
                    aCurObjectMap.erase(pElement);
                }
                //                    IntToSuperAnimObjectMap::iterator anIt = aCurObjectMap.find(anObjectId);
                //                    if (anIt != aCurObjectMap.end())
                //                    {
                //                        aCurObjectMap.erase(anIt);
                //                    }
            }
        }

        if (aFrameFlags & FRAMEFLAGS_ADDS)
        {
            int aNumAdds = aBuffer.ReadByte();
            MYPRINT("Adding %d Frames\n", aNumAdds);
            for(int anAddNum = 0; anAddNum < aNumAdds; ++anAddNum)
            {
                int anObjNum = (aBuffer.ReadShort() & 0x07FF);
                MYPRINT("ObjectID: %d\n", anObjNum);
                SuperAnim::SuperAnimObject& aSuperAnimObject = aCurObjectMap[anObjNum];
                aSuperAnimObject.mObjectNum = anObjNum;
                aSuperAnimObject.mResNum = aBuffer.ReadByte();
                aSuperAnimObject.mColor = SuperAnim::Color(255, 255, 255, 255);
            }
        }

        if (aFrameFlags & FRAMEFLAGS_MOVES)
        {
            int aNumMoves = aBuffer.ReadByte();
            for (int aMoveNum = 0; aMoveNum < aNumMoves; ++ aMoveNum)
            {
                unsigned short aFlagsAndObjectNum = aBuffer.ReadShort();
                int anObjectNum = aFlagsAndObjectNum & 0x03FF;

                SuperAnim::IntToSuperAnimObjectMap::Element *pElement = aCurObjectMap.find(anObjectNum);
                if(pElement == NULL){
                    continue;
                }

                SuperAnim::SuperAnimObject &aSuperAnimObject = pElement->value();//anIt->second;
                MYPRINT("aSuperAnimObject: ObjNum: %d, Res: %d\n", aSuperAnimObject.mObjectNum, aSuperAnimObject.mResNum);
                aSuperAnimObject.mTransform.mMatrix.LoadIdentity();

                if (aFlagsAndObjectNum & MOVEFLAGS_MATRIX)
                {
                    aSuperAnimObject.mTransform.mMatrix.m00 = aBuffer.ReadLong() / LONG_TO_FLOAT;
                    aSuperAnimObject.mTransform.mMatrix.m01 = -aBuffer.ReadLong() / LONG_TO_FLOAT;
                    aSuperAnimObject.mTransform.mMatrix.m10 = -aBuffer.ReadLong() / LONG_TO_FLOAT;
                    aSuperAnimObject.mTransform.mMatrix.m11 = aBuffer.ReadLong() / LONG_TO_FLOAT;
                }
                else if (aFlagsAndObjectNum & MOVEFLAGS_ROTATE)
                {
                    float aRot = aBuffer.ReadShort() / 1000.0f;
                    float sinRot = sinf(aRot);
                    float cosRot = cosf(aRot);
                    aSuperAnimObject.mTransform.mMatrix.m00 = cosRot;
                    aSuperAnimObject.mTransform.mMatrix.m01 = sinRot;
                    aSuperAnimObject.mTransform.mMatrix.m10 = -sinRot;
                    aSuperAnimObject.mTransform.mMatrix.m11 = cosRot;
                }

                SuperAnim::SuperAnimMatrix3 aMatrix;
                aMatrix.LoadIdentity();
                if (aFlagsAndObjectNum & MOVEFLAGS_LONGCOORDS)
                {
                    aMatrix.m02 = aBuffer.ReadLong() / TWIPS_PER_PIXEL;
                    aMatrix.m12 = aBuffer.ReadLong() / TWIPS_PER_PIXEL;
                }
                else
                {
                    aMatrix.m02 = aBuffer.ReadShort() / TWIPS_PER_PIXEL;
                    aMatrix.m12 = aBuffer.ReadShort() / TWIPS_PER_PIXEL;
                }
                aSuperAnimObject.mTransform.mMatrix = aMatrix * aSuperAnimObject.mTransform.mMatrix;

                if (aFlagsAndObjectNum & MOVEFLAGS_COLOR)
                {
                    aSuperAnimObject.mColor.mRed = aBuffer.ReadByte();
                    aSuperAnimObject.mColor.mGreen = aBuffer.ReadByte();
                    aSuperAnimObject.mColor.mBlue = aBuffer.ReadByte();
                    aSuperAnimObject.mColor.mAlpha = aBuffer.ReadByte();
                    MYPRINT("Color Transform: %d %d %d %d\n", aSuperAnimObject.mColor.mRed, aSuperAnimObject.mColor.mGreen,
                            aSuperAnimObject.mColor.mBlue, aSuperAnimObject.mColor.mAlpha);
                }
            }
        }

        if (aFrameFlags & FRAMEFLAGS_FRAME_NAME)
        {
            String aFrameName = aBuffer.ReadString();
            print_line("FrameName: " + aFrameName);
            SuperAnim::SuperAnimLabel aLabel;
            aLabel.mLabelName = aFrameName;
            aLabel.mStartFrameNum = aFrameNum;
            //aMainDef.mLabels.insert(StringToIntMap::value_type(aFrameName, aFrameNum));
            aSuperAnimLabelArray.push_back(aLabel);
        }

        aFrame.mObjectVector.resize(aCurObjectMap.size());
        aFrame.mObjectVector.clear();

        for(SuperAnim::IntToSuperAnimObjectMap::Element *pElement = aCurObjectMap.front();
            pElement != NULL;
            pElement = pElement->next()){
            SuperAnim::SuperAnimObject &anObject = pElement->value();
            aFrame.mObjectVector.push_back(anObject);
        }
    }

    // sort the label array & calculate the end frame for each label
    aSuperAnimLabelArray.sort();

    if (aSuperAnimLabelArray.size() > 1) {
        for (int i = 0; i < aSuperAnimLabelArray.size() - 1; i++) {
            SuperAnim::SuperAnimLabel& aCurLabel = aSuperAnimLabelArray[i];
            const SuperAnim::SuperAnimLabel& aNextLabel = aSuperAnimLabelArray[i + 1];
            aCurLabel.mEndFrameNum = aNextLabel.mStartFrameNum - 1;
        }
        SuperAnim::SuperAnimLabel& aLastLabel = aSuperAnimLabelArray[aSuperAnimLabelArray.size() - 1];
        aLastLabel.mEndFrameNum = aMainDef.mEndFrameNum;
    } else {
        // only have one section
        SuperAnim::SuperAnimLabel& aLabel = aSuperAnimLabelArray[0];
        aLabel.mEndFrameNum = aMainDef.mEndFrameNum;
    }
    aMainDef.mLabels.clear();
    for (int i = 0; i < aSuperAnimLabelArray.size(); i++) {
        aMainDef.mLabels.push_back(aSuperAnimLabelArray[i]);

        SuperAnim::SuperAnimLabel& aCurLabel = aSuperAnimLabelArray[i];
        print_line("CurLabel: " + aCurLabel.mLabelName + " sf:" + itos(aCurLabel.mStartFrameNum) + " ef:" + itos(aCurLabel.mEndFrameNum));
    }

    return true;
}

RES SuperAnimDataFormatLoader::load(const String &p_path,const String& p_original_path)
{
    print_line("PathIs: " + p_path);
    SuperAnimData *sad = memnew(SuperAnimData);
    
    sad->LoadSuperAnimMainDef(p_path);

    Ref<SuperAnimData> animres(sad);

    if(animres.is_null()){
        MYPRINT("Anim Res is NULL!!");
    }

    SuperAnim::SuperAnimDefMgr::GetInstance()->addMainDef(p_path, sad);

    return animres;
}

void SuperAnimDataFormatLoader::get_recognized_extensions(List<String> *p_extensions) const
{
    p_extensions->push_back("sam");
}

bool SuperAnimDataFormatLoader::handles_type(const String& p_type) const
{
    return (p_type == "sam" || p_type == "SuperAnim");
}

String SuperAnimDataFormatLoader::get_resource_type(const String &p_path) const
{
    print_line("SuperAnimDataFormatLoader::get_resource_type()");
	String el = p_path.extension().to_lower();
    if(el == "sam"){
        return "SuperAnim";
    }
    return "";
}
