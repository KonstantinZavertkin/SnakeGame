#include "TDrawer.h"


namespace io_submodule
{
    TDrawer::TDrawer( TRenderer& rendererRef ) : renderer( rendererRef )
    {
    }

    TDrawer::~TDrawer()
    {
    }

    void TDrawer::addPrimitive( TRectangleDescription primitiveDescription )
    {
        staticPrimitives.push_back( primitiveDescription );
    }

    void TDrawer::addField( TFieldDrawer* fieldDrawerPtr )
    {
        fieldDrawers.push_back( fieldDrawerPtr );
    }

    void TDrawer::addText( TFontDrawer* textPrinterPtr )
    {
        textPrinters.push_back( textPrinterPtr );
    }

    TRenderer& TDrawer::getRendererRef()
    {
        return renderer;
    }

    void TDrawer::draw()
    {
        for ( auto& primitive : staticPrimitives )
            renderer.draw( primitive );

        for ( auto& field : fieldDrawers )
           field->drawField();

        for ( auto& textPrinter : textPrinters )
           textPrinter->drawText();

    }
};