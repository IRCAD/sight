/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

 #include <string>
 #include <iomanip>
 #include <sstream>

#include "midasIO/ModelSeriesWriter.hpp"

#include <fwCore/base.hpp>

#include <fwData/Material.hpp>
#include <fwData/StructureTraitsDictionary.hpp>

#include <fwDataIO/reader/DictionaryReader.hpp>
#include <fwDataIO/writer/registry/macros.hpp>

#include <fwVtkIO/helper/Mesh.hpp>

#include <vtkDataSetMapper.h>
#include <vtkOBJExporter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <boost/algorithm/string/replace.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/lambda/lambda.hpp>

#include <libxml/parser.h>
#include <libxml/tree.h>

fwDataIOWriterRegisterMacro( ::midasIO::ModelSeriesWriter );


namespace midasIO
{

const ModelSeriesWriter::OrganClass2LayerType ModelSeriesWriter::s_ORGAN_LAYERS
    = ::boost::assign::map_list_of
          (::fwData::StructureTraits::TOOL, "1")
          (::fwData::StructureTraits::NO_CONSTRAINT, "1")
          (::fwData::StructureTraits::LESION, "2")
          (::fwData::StructureTraits::FUNCTIONAL, "2")
          (::fwData::StructureTraits::VESSEL, "3")
          (::fwData::StructureTraits::ORGAN, "4")
          (::fwData::StructureTraits::ENVIRONMENT, "5");

//------------------------------------------------------------------------------

ModelSeriesWriter::ModelSeriesWriter(::fwDataIO::writer::IObjectWriter::Key key)
    : ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this)
{
}

//------------------------------------------------------------------------------

ModelSeriesWriter::~ModelSeriesWriter()
{
}

//------------------------------------------------------------------------------

std::string castColorVal( double d )
{
    std::stringstream sstr;
    sstr << std::setprecision(4) << d;
    return sstr.str();
}

//------------------------------------------------------------------------------

void ModelSeriesWriter::write()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    std::string organName, layer;

    ::fwMedData::ModelSeries::sptr pModelSeries = getConcreteObject();
    int nbSerie = 0;
    xmlDocPtr xmlDoc;
    xmlNodePtr root_node, item_node;
    xmlDoc    = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "Items");
    xmlDocSetRootElement(xmlDoc, root_node);

    ::fwData::StructureTraitsDictionary::sptr structDico = ::fwData::StructureTraitsDictionary::New();

    ::fwDataIO::reader::DictionaryReader::sptr dictionaryReader = ::fwDataIO::reader::DictionaryReader::New();
    dictionaryReader->setObject(structDico);
    dictionaryReader->setFile( ::fwDataIO::reader::DictionaryReader::getDefaultDictionaryPath() );
    dictionaryReader->read();

    const ::fwData::StructureTraitsDictionary::StructureTypeNameContainer& names = structDico->getStructureTypeNames();

    for( ::fwData::Reconstruction::sptr rec :  pModelSeries->getReconstructionDB() )
    {
        nbSerie++;

        vtkRenderer* renderer = vtkRenderer::New();
        vtkActor* actor       = this->createActor(rec);
        renderer->AddActor(actor);

        vtkRenderWindow* renderWindow = vtkRenderWindow::New();
        renderWindow->AddRenderer(renderer);

        vtkOBJExporter* exporter = vtkOBJExporter::New();
        exporter->SetRenderWindow(renderWindow);

        std::string fileName = "TriangularMesh_" + ::fwTools::getString< int >(nbSerie);
        exporter->SetFilePrefix((this->getFolder().string() + "/" + fileName).c_str());
        exporter->Write();

        // retrieves reconstruction color
        ::fwData::Material::sptr material = rec->getMaterial();
        ::fwData::Color::sptr color       = material->diffuse();
        double red   = color->red();
        double green = color->green();
        double blue  = color->blue();

        if(std::find(names.begin(), names.end(), rec->getStructureType()) != names.end())
        {
            ::fwData::StructureTraits::sptr structTraits = structDico->getStructure(rec->getStructureType());
            OrganClass2LayerType::const_iterator it = s_ORGAN_LAYERS.find(structTraits->getClass());
            layer = it->second;
        }
        else
        {
            layer = "1";
        }

        organName = rec->getOrganName();

        ::boost::replace_all(organName, " ", "_"); // "Name" must not contains whitespaces

        // add the reconstruction to the XML file
        item_node = xmlNewChild(root_node, NULL, BAD_CAST "Item", NULL);
        xmlNewChild(item_node, NULL, BAD_CAST "Name", BAD_CAST organName.c_str());
        xmlNewChild(item_node, NULL, BAD_CAST "Red", BAD_CAST ( castColorVal(red) ).c_str());
        xmlNewChild(item_node, NULL, BAD_CAST "Green", BAD_CAST ( castColorVal(green) ).c_str());
        xmlNewChild(item_node, NULL, BAD_CAST "Blue", BAD_CAST ( castColorVal(blue) ).c_str());
        xmlNewChild(item_node, NULL, BAD_CAST "FileName", BAD_CAST ( fileName + ".obj" ).c_str() );
        xmlNewChild(item_node, NULL, BAD_CAST "Layer", BAD_CAST layer.c_str());
        xmlNewChild(item_node, NULL, BAD_CAST "IsVisible",
                    BAD_CAST ( ::fwTools::getString< bool > (rec->getIsVisible()) ).c_str());
        xmlNewChild(item_node, NULL, BAD_CAST "Url", NULL);

        actor->Delete();
        renderer->Delete();
        renderWindow->Delete();
        exporter->Delete();
    }

    xmlSaveFormatFileEnc((this->getFolder().string() + "/sceneDescription.xml").c_str(), xmlDoc, "UTF-8", 1);
    xmlFreeDoc(xmlDoc);
}

//------------------------------------------------------------------------------

vtkActor* ModelSeriesWriter::createActor( ::fwData::Reconstruction::sptr pReconstruction )
{
    vtkActor* actor = vtkActor::New();

    ::fwData::Mesh::sptr mesh         = pReconstruction->getMesh();
    ::fwData::Material::sptr material = pReconstruction->getMaterial();

    vtkSmartPointer< vtkPolyData > polyData = vtkSmartPointer< vtkPolyData >::New();
    ::fwVtkIO::helper::Mesh::toVTKMesh( mesh, polyData);
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);
    actor->SetMapper(mapper);

    vtkProperty* property = actor->GetProperty();

    ::fwData::Color::sptr diffuse = material->diffuse();
    property->SetDiffuseColor(diffuse->red(), diffuse->green(), diffuse->blue());
    property->SetOpacity( diffuse->alpha() );

    ::fwData::Color::sptr ambient = material->ambient();
    property->SetAmbientColor(ambient->red(), ambient->green(), ambient->blue());

    property->SetSpecularColor(1.,1.,1.);
    property->SetSpecularPower(100.); //Shininess

    property->SetInterpolationToPhong();

    return actor;
}

//------------------------------------------------------------------------------

std::string ModelSeriesWriter::extension()
{
    return "";
}

} // namespace midasIO
