<?php

function msdc_alpha_preprocess_html( &$vars ) {
    if( $_SERVER['REQUEST_URI'] == base_path() ) {
        drupal_add_js( base_path()."/sites/default/themes/msdc/js/html5logo.js" );
    }
}

