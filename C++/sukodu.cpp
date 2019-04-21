//
//  main.cpp
//  sokudo
//
//  Created by cuidx on 16/8/13.
//  Copyright © 2016年 cuidx. All rights reserved.
//

#include <iostream>
#include <set>
#include <map>
#include <list>


#define DEF_ROW     9
#define DEF_COL     9
#define IN_SIZE     (DEF_ROW * DEF_COL)

typedef struct ItemSetableInfo
{
    int m_xPos;
    int m_yPos;
    std::set<char> m_setableVals;
}ItemSetableInfo;

typedef std::list<ItemSetableInfo> ItemInfoList;
typedef std::map<int,ItemInfoList> SortItemInfo;


bool GetSetableValue(char inBox[IN_SIZE],int xPos,int yPos,std::set<char> &vals)
{
    vals.clear();
    if (xPos >= DEF_COL || yPos >= DEF_ROW)
    {
        return false;
    }
    if (inBox[yPos * DEF_COL + xPos] > 0)
    {
        return true;
    }
    for (int i = 1; i <= 9; i++)
    {
        vals.insert(i);
    }
    //check row
    for (int iX = 0; iX < DEF_COL; iX++)
    {
        if (iX != xPos)
        {
            std::set<char>::iterator iter = vals.find(inBox[yPos * DEF_COL + iX]);
            if (iter != vals.end())
            {
                vals.erase(iter);
            }
        }
    }
    //check col
    for (int iY = 0; iY < DEF_ROW; iY++)
    {
        if (iY != yPos)
        {
            std::set<char>::iterator iter = vals.find(inBox[iY * DEF_COL + xPos]);
            if (iter != vals.end())
            {
                vals.erase(iter);
            }

        }
    }
    
    //check box
    int xBoxStart = xPos / 3 * 3;
    int yBoxStart = yPos / 3 * 3;
    for (int iRow = yBoxStart; iRow < yBoxStart + 3; iRow++)
    {
        if (iRow == yPos)
        {
            continue;
        }
        for (int iCol = xBoxStart; iCol < xBoxStart + 3; iCol++)
        {
            if (iCol == xPos)
            {
                continue;
            }
            std::set<char>::iterator iter = vals.find(inBox[iRow * DEF_COL + iCol]);
            if (iter != vals.end())
            {
                vals.erase(iter);
            }

        }
    }
    return vals.size() > 0;
}

bool GetAllSetableVals(char inBox[IN_SIZE],SortItemInfo &result)
{
    result.clear();
    
    for (int yPos = 0; yPos < DEF_ROW; yPos++)
    {
        for (int xPos = 0; xPos < DEF_COL; xPos++)
        {
            std::set<char> vals;

            if (GetSetableValue(inBox, xPos, yPos, vals))
            {
                if (vals.size() > 0)
                {
                    ItemSetableInfo info;
                    info.m_xPos = xPos;
                    info.m_yPos = yPos;
                    info.m_setableVals = vals;
                    if (result.find(vals.size()) == result.end())
                    {
                        ItemInfoList infoList;
                        infoList.push_back(info);
                        result[vals.size()] = infoList;
                    }
                    else
                    {
                        result[vals.size()].push_back(info);
                    }
                }
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}

bool CheckBoxValid(char inBox[IN_SIZE])
{
    std::set<char> vals;
    std::map<int, std::set<char> > colVals;
    std::map<int, std::set<char> > boxVals;
    
    //check col
    for (int iY = 0; iY < DEF_ROW; iY++)
    {
        //check row
        for (int iX = 0; iX < DEF_COL; iX++)
        {
            if (inBox[iY * DEF_COL + iX] > 0)
            {
                if (vals.find(inBox[iY * DEF_COL + iX]) == vals.end())
                {
                    vals.insert(inBox[iY * DEF_COL + iX]);
                }
                else
                {
                    return false;
                }
                if (colVals[iX].find(inBox[iY * DEF_COL + iX]) == colVals[iX].end())
                {
                    colVals[iX].insert(inBox[iY * DEF_COL + iX]);
                }
                else
                {
                    return false;
                }
                int iBox = iY / 3 * 3 + iX / 3;
                if (boxVals[iBox].find(inBox[iY * DEF_COL + iX]) == boxVals[iBox].end())
                {
                    boxVals[iBox].insert(inBox[iY * DEF_COL + iX]);
                }
                else
                {
                    return false;
                }
            }
        }
        vals.clear();
        
        
    }
    
    return true;
}


bool DoSokudu(char inBox[IN_SIZE])
{
    SortItemInfo result;
    bool bRet = GetAllSetableVals(inBox, result);
    if(!bRet)
    {
        return false;
    }
    
    while (result.size() > 0)
    {
        SortItemInfo::iterator iterOne = result.find(1);
        if (result.end() != iterOne)
        {
            ItemInfoList::iterator itrList = iterOne->second.begin();
            for (; itrList != iterOne->second.end(); itrList++)
            {
                //printf("OOR:[x:%d,y:%d:val:%d]\n",itrList->m_xPos,itrList->m_yPos,*(itrList->m_setableVals.begin()));
                inBox[itrList->m_yPos * DEF_COL + itrList->m_xPos] = *(itrList->m_setableVals.begin());
            }
            if (!CheckBoxValid(inBox))
            {
                return false;
            }
            bRet = GetAllSetableVals(inBox, result);
            if (!bRet)
            {
                return false;
            }
            continue;
        }
        else
        {
            //printf("test val\n");
            SortItemInfo::iterator iterFirst = result.begin();
            ItemInfoList valList = iterFirst->second;
            if (valList.size() > 0)
            {
                char tempInBox[IN_SIZE];
                ItemInfoList::iterator itrList = valList.begin();
                //printf("TIC:[x:%d,y:%d,val:]",itrList->m_xPos,itrList->m_yPos);
                //std::set<char>::iterator itVal = itrList->m_setableVals.begin();
                //for (;itVal != itrList->m_setableVals.end();itVal++)
                //{
                //    printf("%d ",*itVal);
                //}
                //printf("\n");
                while (itrList->m_setableVals.size() > 0)
                {
                    memcpy(tempInBox,inBox,IN_SIZE);
                    tempInBox[itrList->m_yPos * DEF_COL + itrList->m_xPos] = *(itrList->m_setableVals.begin());
                    itrList->m_setableVals.erase(itrList->m_setableVals.begin());
                    
                    if (!DoSokudu(tempInBox))
                    {
                        //printf("TTC:[x:%d,y:%d,val:%d] fail\n",itrList->m_xPos,itrList->m_yPos,tempInBox[itrList->m_yPos * DEF_COL + itrList->m_xPos]);
                        continue;
                    }
                    else
                    {
                        //printf("TTC:[x:%d,y:%d,val:%d] succ\n",itrList->m_xPos,itrList->m_yPos,tempInBox[itrList->m_yPos * DEF_COL + itrList->m_xPos]);
#if 0
                        memcpy(inBox,tempInBox,IN_SIZE);
                        return true;
#else
                        printf("sokudo result:\n");
                        for (int iRow = 0; iRow < DEF_ROW; iRow++)
                        {
                            for (int iCol = 0; iCol < DEF_COL; iCol++)
                            {
                                printf("%d ",tempInBox[iRow * DEF_COL + iCol]);
                            }
                            printf("\n");
                        }
                        printf("------------------------\n");

                        continue;
#endif
                    }
                }
            }
            return false;
        }
    }
    return bRet;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    char lszInBox[IN_SIZE] =
    {
        6, 0, 0, 0, 7, 0, 9, 4, 0,
        8, 0, 0, 0, 1, 0, 2, 0, 7,
        0, 4, 7, 8, 0, 0, 0, 0, 0,
        9, 0, 8, 0, 6, 0, 4, 0, 3,
        0, 6, 0, 1, 0, 0, 0, 8, 0,
        0, 0, 0, 0, 0, 8, 6, 0, 2,
        4, 0, 6, 0, 2, 0, 0, 9, 5,
        0, 9, 0, 0, 0, 0, 3, 0, 0,
        3, 0, 0, 6, 0, 0, 1, 0, 4
    };
/*
    char lszInBox[IN_SIZE] =
    {
        6, 0, 0, 0, 7, 0, 9, 4, 8,
        8, 0, 0, 0, 1, 0, 2, 0, 7,
        2, 4, 7, 8, 0, 0, 5, 0, 1,
        9, 0, 8, 0, 6, 0, 4, 0, 3,
        5, 6, 0, 1, 0, 0, 7, 8, 9,
        0, 0, 0, 0, 0, 8, 6, 0, 2,
        4, 0, 6, 0, 2, 0, 8, 9, 5,
        0, 9, 0, 0, 0, 0, 3, 0, 6,
        3, 0, 0, 6, 0, 0, 1, 0, 4
    };

    char lszInBox[IN_SIZE] =
    {
        0, 0, 9, 8, 0, 0, 0, 0, 0,
        4, 0, 0, 0, 0, 0, 0, 3, 0,
        0, 0, 0, 0, 6, 0, 7, 0, 4,
        0, 0, 3, 0, 0, 0, 0, 2, 0,
        0, 0, 0, 0, 0, 3, 5, 0, 0,
        0, 8, 2, 7, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 9, 5, 1, 0, 0,
        0, 9, 1, 3, 8, 0, 0, 0, 6,
        3, 0, 7, 0, 0, 0, 9, 0, 0
    };

    char lszInBox[IN_SIZE] =
    {
        2, 0, 0, 0, 8, 0, 4, 0, 0,
        0, 0, 6, 0, 0, 4, 0, 0, 5,
        8, 4, 0, 0, 7, 0, 2, 0, 1,
        0, 7, 0, 0, 0, 2, 0, 0, 0,
        0, 0, 1, 0, 9, 0, 5, 2, 0,
        5, 8, 0, 6, 0, 1, 9, 0, 4,
        0, 0, 3, 0, 4, 0, 0, 0, 2,
        9, 0, 0, 3, 0, 0, 1, 0, 0,
        0, 6, 0, 0, 2, 5, 0, 0, 9
    };
 */
    clock_t tStart = clock();
    DoSokudu(lszInBox);
    clock_t tEnd = clock();
    printf("Time used:%f",(tEnd - tStart) * 1.0 / CLOCKS_PER_SEC);
#if 0
    printf("sokudo result:\n");
    for (int iRow = 0; iRow < DEF_ROW; iRow++)
    {
        for (int iCol = 0; iCol < DEF_COL; iCol++)
        {
            printf("%d ",lszInBox[iRow * DEF_COL + iCol]);
        }
        printf("\n");
    }
#endif
    //std::cout << "Hello, World!\n";
    //getchar();
    return 0;
}
