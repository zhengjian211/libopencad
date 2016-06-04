/*******************************************************************************
 *  Project: libopencad
 *  Purpose: OpenSource CAD formats support library
 *  Author: Alexandr Borzykh, mush3d at gmail.com
 *  Author: Dmitry Baryshnikov, bishop.dev@gmail.com
 *  Language: C++
 *******************************************************************************
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2016 Alexandr Borzykh
 *  Copyright (c) 2016 NextGIS, <info@nextgis.com>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *******************************************************************************/

#include "opencad_api.h"
#include "cadgeometry.h"

#include <cstddef>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory>

using namespace std;

static int Usage(const char* pszErrorMsg = nullptr)
{
    cout << "Usage: cadinfo [--help] [--formats][--version]\n"
            "               file_name" << endl;

    if( pszErrorMsg != nullptr )
    {
        cerr << endl << "FAILURE: " << pszErrorMsg << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int Version()
{
    cout << "cadinfo was compiled against libopencad "
         << OCAD_VERSION << endl
         << "and is running against libopencad "
         << GetVersionString() << endl;

    return EXIT_SUCCESS;
}

static int Formats()
{
    cerr << GetCADFormats() << endl;

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if( argc < 1 )
       return -argc;

    const char  *pszCADFilePath = nullptr;

    for( int iArg = 1; iArg < argc; ++iArg)
    {
        if (strcmp(argv[iArg],"-h")==0 || strcmp(argv[iArg],"--help")==0)
        {
            return Usage();
        }
        else if(strcmp(argv[iArg],"-f")==0 || strcmp(argv[iArg],"--formats")==0)
        {
            return Formats ();
        }
        else if(strcmp(argv[iArg],"-v")==0 || strcmp(argv[iArg],"--version")==0)
        {
            return Version();
        }
        else
        {
            pszCADFilePath = argv[iArg];
        }
    }

    CADFile *pCADFile = OpenCADFile( pszCADFilePath,
                                      CADFile::OpenOptions::READ_ALL );

    if (pCADFile == nullptr)
    {
        cerr << "Open CAD file " << pszCADFilePath << " failed." << endl;
        return EXIT_FAILURE;
    }

    const CADHeader& header = pCADFile->getHeader ();
    header.print ();
    cout << endl;

    const CADClasses& classes = pCADFile->getClasses ();
    classes.print ();
    cout << endl;

    int solids_count = 0;
    int splines_count = 0;
    int circles_count = 0;
    int lines_count = 0;
    int ellipses_count = 0;
    int pline_count = 0;
    int pline3d_count = 0;
    int point_count = 0;
    int arc_count = 0;
    int text_count = 0;
    cout << "Layers count: " << pCADFile->getLayersCount () << endl;

    size_t i,j;
    for ( i = 0; i < pCADFile->getLayersCount (); ++i )
    {
        CADLayer &layer = pCADFile->getLayer (i);
        cout << "Layer #" << i << " contains "
             << layer.getGeometryCount () << " geometries" << endl;

        for ( j = 0; j < layer.getGeometryCount (); ++j )
        {
            unique_ptr<CADGeometry> geom(layer.getGeometry (j));

            if ( geom != nullptr )
            {
                geom->print ();

                switch ( geom->getType() )
                {
                    case CADGeometry::CIRCLE:
                        ++circles_count;
                        break;

                    case CADGeometry::LWPOLYLINE:
                        ++pline_count;
                        break;

                    case CADGeometry::POLYLINE3D:                    
                        ++pline3d_count;
                        break;

                    case CADGeometry::ARC:
                        ++arc_count;
                        break;
                    case CADGeometry::POINT:
                        ++point_count;
                        break;
                    case CADGeometry::ELLIPSE:
                        ++ellipses_count;
                        break;

                    case CADGeometry::LINE:
                        ++lines_count;
                        break;
                    case CADGeometry::SPLINE:
                        ++splines_count;
                        break;

                    case CADGeometry::TEXT:
                        ++text_count;
                        break;

                    case CADGeometry::SOLID:
                        ++solids_count;
                        break;
                }
            }
        }
    }

    cout << "Solids: " << solids_count << endl;
    cout << "Points: " << point_count << endl;
    cout << "Ellipses: " << ellipses_count << endl;
    cout << "Lines count: " << lines_count << endl;
    cout << "Plines count: " << pline_count << endl;
    cout << "Plines3d count: " << pline3d_count << endl;
    cout << "Splines count: " << splines_count << endl;
    cout << "Circles count: " << circles_count << endl;
    cout << "Arcs count: " << arc_count << endl;
    cout << "Texts count: " << text_count << endl;
}
