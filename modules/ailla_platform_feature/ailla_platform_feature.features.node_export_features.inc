<?php
/**
 * @file
 * ailla_platform_feature.features.node_export_features.inc
 */

/**
 * Implements hook_node_export_features_default().
 */
function ailla_platform_feature_node_export_features_default() {
  $node_export = array(
  'code_string' => 'array(
  (object) array(
      \'vid\' => \'2\',
      \'uid\' => \'1\',
      \'title\' => \'Test Two\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'0\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'be76d38e-6514-472c-a65d-428c1b502ecf\',
      \'nid\' => \'2\',
      \'type\' => \'page\',
      \'language\' => \'en\',
      \'created\' => \'1432748652\',
      \'changed\' => \'1432748652\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'10c14aa6-5068-4b29-b3bd-e5843e5e8c3b\',
      \'revision_timestamp\' => \'1432748652\',
      \'revision_uid\' => \'1\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => " Nunc condimentum nisl diam, ut sollicitudin tellus mattis in. Pellentesque ultrices est ultrices fringilla luctus. Fusce at odio neque. Vestibulum semper orci id maximus faucibus. Etiam vel sem rhoncus, pulvinar odio a, mollis nisl. Nullam faucibus leo luctus lorem interdum mattis. Fusce sed leo ut velit bibendum malesuada sit amet at neque. Nunc luctus congue magna vitae finibus. Ut eu mi sodales, gravida nulla ac, facilisis nisl. Nulla facilisi.\\r\\n\\r\\nNullam blandit commodo mi, semper elementum lacus consequat eu. Donec pretium elit nibh, nec vestibulum lacus rhoncus non. Duis molestie, mauris at congue hendrerit, augue risus ultricies nunc, a volutpat tellus arcu vitae dui. Praesent a sodales risus, in aliquet eros. In maximus velit est, non feugiat est vehicula eu. Vestibulum sit amet neque ligula. Aenean vel elit iaculis, maximus tortor dapibus, tristique sem. Praesent ut arcu et metus tincidunt rutrum. Nulla facilisi. Integer in congue purus. Curabitur quis auctor orci, non feugiat dui. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Aenean congue est et ornare sollicitudin. ",
            \'summary\' => \'\',
            \'format\' => \'filtered_html\',
            \'safe_value\' => "<p>Nunc condimentum nisl diam, ut sollicitudin tellus mattis in. Pellentesque ultrices est ultrices fringilla luctus. Fusce at odio neque. Vestibulum semper orci id maximus faucibus. Etiam vel sem rhoncus, pulvinar odio a, mollis nisl. Nullam faucibus leo luctus lorem interdum mattis. Fusce sed leo ut velit bibendum malesuada sit amet at neque. Nunc luctus congue magna vitae finibus. Ut eu mi sodales, gravida nulla ac, facilisis nisl. Nulla facilisi.</p>\\n<p>Nullam blandit commodo mi, semper elementum lacus consequat eu. Donec pretium elit nibh, nec vestibulum lacus rhoncus non. Duis molestie, mauris at congue hendrerit, augue risus ultricies nunc, a volutpat tellus arcu vitae dui. Praesent a sodales risus, in aliquet eros. In maximus velit est, non feugiat est vehicula eu. Vestibulum sit amet neque ligula. Aenean vel elit iaculis, maximus tortor dapibus, tristique sem. Praesent ut arcu et metus tincidunt rutrum. Nulla facilisi. Integer in congue purus. Curabitur quis auctor orci, non feugiat dui. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Aenean congue est et ornare sollicitudin.</p>\\n",
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1432748652\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'1\',
      \'comment_count\' => \'0\',
      \'name\' => \'islandora\',
      \'picture\' => \'0\',
      \'data\' => \'b:0;\',
      \'path\' => FALSE,
      \'menu\' => array(
        \'link_title\' => \'Rights\',
        \'mlid\' => 0,
        \'plid\' => \'609\',
        \'menu_name\' => \'main-menu\',
        \'weight\' => \'-50\',
        \'options\' => array(),
        \'module\' => \'menu\',
        \'expanded\' => \'1\',
        \'hidden\' => \'0\',
        \'has_children\' => \'1\',
        \'customized\' => 0,
        \'parent_depth_limit\' => 8,
        \'description\' => \'\',
        \'enabled\' => 1,
      ),
      \'node_export_drupal_version\' => \'7\',
    ),
  (object) array(
      \'vid\' => \'1\',
      \'uid\' => \'1\',
      \'title\' => \'Help\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'0\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'a7acebcf-5b58-463e-af85-2055add037c9\',
      \'nid\' => \'1\',
      \'type\' => \'page\',
      \'language\' => \'en\',
      \'created\' => \'1432746479\',
      \'changed\' => \'1433168982\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'1df574dc-04ac-44d7-87ff-798020aafbc0\',
      \'revision_timestamp\' => \'1433168982\',
      \'revision_uid\' => \'1\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => \'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam ultricies nec dolor non dapibus. Praesent at tellus a sapien vestibulum placerat. Fusce mi dui, faucibus id turpis a, malesuada convallis quam. Phasellus convallis justo vel auctor interdum. Donec hendrerit nulla dui, sit amet fringilla eros sollicitudin at. Suspendisse eget felis non ante commodo pharetra. Interdum et malesuada fames ac ante ipsum primis in faucibus. Suspendisse mi est, pellentesque non luctus sit amet, tincidunt nec elit. Integer suscipit eleifend tortor nec pharetra. Cras bibendum tristique dui, nec sagittis diam. Nulla nunc orci, elementum scelerisque leo eu, tincidunt semper ante.\',
            \'summary\' => \'\',
            \'format\' => \'filtered_html\',
            \'safe_value\' => "<p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam ultricies nec dolor non dapibus. Praesent at tellus a sapien vestibulum placerat. Fusce mi dui, faucibus id turpis a, malesuada convallis quam. Phasellus convallis justo vel auctor interdum. Donec hendrerit nulla dui, sit amet fringilla eros sollicitudin at. Suspendisse eget felis non ante commodo pharetra. Interdum et malesuada fames ac ante ipsum primis in faucibus. Suspendisse mi est, pellentesque non luctus sit amet, tincidunt nec elit. Integer suscipit eleifend tortor nec pharetra. Cras bibendum tristique dui, nec sagittis diam. Nulla nunc orci, elementum scelerisque leo eu, tincidunt semper ante.</p>\\n",
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1432746479\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'1\',
      \'comment_count\' => \'0\',
      \'name\' => \'islandora\',
      \'picture\' => \'0\',
      \'data\' => \'b:0;\',
      \'path\' => FALSE,
      \'menu\' => NULL,
      \'node_export_drupal_version\' => \'7\',
    ),
  (object) array(
      \'vid\' => \'10\',
      \'uid\' => \'1\',
      \'title\' => \'How To Use\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'0\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'3361a5ae-8d98-46bb-b83e-f0fd35ce1a8b\',
      \'nid\' => \'10\',
      \'type\' => \'page\',
      \'language\' => \'en\',
      \'created\' => \'1432749388\',
      \'changed\' => \'1432749388\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'27f2f019-65d3-4569-965a-5afb045927ed\',
      \'revision_timestamp\' => \'1432749388\',
      \'revision_uid\' => \'1\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => "dit odio, posuere cursus enim gravida id. Integer ut turpis quis ligula eleifend porttitor. Donec in fringilla metus. Proin bibendum sagittis ligula, eu imperdiet magna molestie eu. Donec eu lectus massa. Pellentesque sit amet tortor mi. Quisque nec elit quis eros fringilla fermentum. Suspendisse porttitor elit quis eros ultricies malesuada. Nunc sollicitudin aliquam mauris, at scelerisque ante suscipit in.\\r\\n\\r\\nPellentesque vestibulum lobortis purus, eget tincidunt massa viverra sit amet. Aenean a magna velit. Cras vulputate dolor leo, et tincidunt nulla ultricies id. Suspendisse euismod, lectus sit amet luctus luctus, velit dolor feugiat lorem, id bibendum neque lorem vel purus. Nulla facilisi. Aenean convallis mauris nec est maximus sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elem",
            \'summary\' => \'\',
            \'format\' => \'filtered_html\',
            \'safe_value\' => "<p>dit odio, posuere cursus enim gravida id. Integer ut turpis quis ligula eleifend porttitor. Donec in fringilla metus. Proin bibendum sagittis ligula, eu imperdiet magna molestie eu. Donec eu lectus massa. Pellentesque sit amet tortor mi. Quisque nec elit quis eros fringilla fermentum. Suspendisse porttitor elit quis eros ultricies malesuada. Nunc sollicitudin aliquam mauris, at scelerisque ante suscipit in.</p>\\n<p>Pellentesque vestibulum lobortis purus, eget tincidunt massa viverra sit amet. Aenean a magna velit. Cras vulputate dolor leo, et tincidunt nulla ultricies id. Suspendisse euismod, lectus sit amet luctus luctus, velit dolor feugiat lorem, id bibendum neque lorem vel purus. Nulla facilisi. Aenean convallis mauris nec est maximus sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elem</p>\\n",
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1432749388\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'1\',
      \'comment_count\' => \'0\',
      \'name\' => \'islandora\',
      \'picture\' => \'0\',
      \'data\' => \'b:0;\',
      \'path\' => FALSE,
      \'menu\' => NULL,
      \'node_export_drupal_version\' => \'7\',
    ),
  (object) array(
      \'vid\' => \'6\',
      \'uid\' => \'1\',
      \'title\' => \'Languages\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'0\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'65fe61df-a90b-4954-af64-e5204f166db4\',
      \'nid\' => \'6\',
      \'type\' => \'page\',
      \'language\' => \'en\',
      \'created\' => \'1432749186\',
      \'changed\' => \'1432749186\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'65a75655-f56c-4e73-a8c7-2b52050e8eb6\',
      \'revision_timestamp\' => \'1432749186\',
      \'revision_uid\' => \'1\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => "dit odio, posuere cursus enim gravida id. Integer ut turpis quis ligula eleifend porttitor. Donec in fringilla metus. Proin bibendum sagittis ligula, eu imperdiet magna molestie eu. Donec eu lectus massa. Pellentesque sit amet tortor mi. Quisque nec elit quis eros fringilla fermentum. Suspendisse porttitor elit quis eros ultricies malesuada. Nunc sollicitudin aliquam mauris, at scelerisque ante suscipit in.\\r\\n\\r\\nPellentesque vestibulum lobortis purus, eget tincidunt massa viverra sit amet. Aenean a magna velit. Cras vulputate dolor leo, et tincidunt nulla ultricies id. Suspendisse euismod, lectus sit amet luctus luctus, velit dolor feugiat lorem, id bibendum neque lorem vel purus. Nulla facilisi. Aenean convallis mauris nec est maximus sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elem",
            \'summary\' => \'\',
            \'format\' => \'filtered_html\',
            \'safe_value\' => "<p>dit odio, posuere cursus enim gravida id. Integer ut turpis quis ligula eleifend porttitor. Donec in fringilla metus. Proin bibendum sagittis ligula, eu imperdiet magna molestie eu. Donec eu lectus massa. Pellentesque sit amet tortor mi. Quisque nec elit quis eros fringilla fermentum. Suspendisse porttitor elit quis eros ultricies malesuada. Nunc sollicitudin aliquam mauris, at scelerisque ante suscipit in.</p>\\n<p>Pellentesque vestibulum lobortis purus, eget tincidunt massa viverra sit amet. Aenean a magna velit. Cras vulputate dolor leo, et tincidunt nulla ultricies id. Suspendisse euismod, lectus sit amet luctus luctus, velit dolor feugiat lorem, id bibendum neque lorem vel purus. Nulla facilisi. Aenean convallis mauris nec est maximus sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elem</p>\\n",
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1432749186\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'1\',
      \'comment_count\' => \'0\',
      \'name\' => \'islandora\',
      \'picture\' => \'0\',
      \'data\' => \'b:0;\',
      \'path\' => FALSE,
      \'menu\' => array(
        \'link_title\' => \'Languages\',
        \'mlid\' => 0,
        \'plid\' => \'609\',
        \'menu_name\' => \'main-menu\',
        \'weight\' => \'-49\',
        \'options\' => array(),
        \'module\' => \'menu\',
        \'expanded\' => \'0\',
        \'hidden\' => \'0\',
        \'has_children\' => \'1\',
        \'customized\' => 0,
        \'parent_depth_limit\' => 8,
        \'description\' => \'\',
        \'enabled\' => 1,
      ),
      \'node_export_drupal_version\' => \'7\',
    ),
  (object) array(
      \'vid\' => \'11\',
      \'uid\' => \'1\',
      \'title\' => \'Welcome to AILLA\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'1\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'11dafb27-c4ed-44fa-a383-e733c2b52157\',
      \'nid\' => \'11\',
      \'type\' => \'page\',
      \'language\' => \'en\',
      \'created\' => \'1432818192\',
      \'changed\' => \'1432819353\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'67f73be7-1045-44ae-b876-2d475b71d04c\',
      \'revision_timestamp\' => \'1432819353\',
      \'revision_uid\' => \'1\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => "<p>\\r\\n<font color=\\"red\\">\\r\\n<b>ATTENTION:</b>\\r\\n</font>\\r\\nThe latest <a href=\\"http://ailla.utexas.org/site/announcements.html\\">News and Announcements</a> about AILLA can be found <a href=\\"http://ailla.utexas.org/site/announcements.html\\">here</a>.\\r\\n</p>\\r\\n<p></p>\\r\\n<p>AILLA is a digital archive of recordings and texts in and about the indigenous languages of Latin America.</p>\\r\\n<p>\\r\\nAccess to AILLA and its resources is always free of charge. Most of the resources in the collection are available to the public, but some have special <a href=\\"access_restrict.html\\">access restrictions</a>.\\r\\n</p>\\r\\n<p>\\r\\n<b>\\r\\nYou must <a href=\\"../user/login.html\\">Register and Login</a> to open or download any media file.\\r\\n</b>\\r\\n</p>\\r\\n<p>\\r\\n<img align=\\"left\\" alt=\\"animal\\" src=\\"/sites/all/themes/multipurpose/images/tiniest_bunny.gif\\"><b>Users:</b> start by <a href=\\"../search/browse.html?type=menu\\">browsing the catalog</a> or use the navigation menu on the left to find more information about the archive and the indigenous languages of Latin America.\\r\\n</p>\\r\\n<p>\\r\\n<img align=\\"left\\" alt=\\"animal\\" src=\\"/sites/all/themes/multipurpose/images/tiniest_bunny.gif\\"><b>Depositors:</b> If you want to deposit materials in AILLA, <a href=\\"dep_info.html\\">start here</a>.\\r\\n</p>\\r\\n</br><h3>The Collection</h3></br>\\r\\n<p>The heart of the collection is recordings, both audio and video, in a wide range of genres: narratives, chants, oratory, conversations, songs, and more. Many recordings are transcribed and translated into Spanish, English or Portuguese. </p>\\r\\n<p>The archive also contains a wealth of language documentation materials: grammars, dictionaries, ethnographies, and field notes. The collection includes teaching materials for bilingual education and language revitalization programs.</p>\\r\\n<p>\\r\\nWe are committed to serving the indigenous people of Latin America and the scholars who study their languages. We are always interested in hearing from you. Please <a href=\\"mailto:ailla@ailla.utexas.org\\">contact us</a> if you have any questions, comments, or suggestions.\\r\\n</p>\\r\\n<p></p>\\r\\n<img align=\\"left\\" alt=\\"animal\\" src=\\"/sites/all/themes/multipurpose/images/animal.gif\\">\\r\\n<br>",
            \'summary\' => \'\',
            \'format\' => \'full_html\',
            \'safe_value\' => "<p>\\n<font color=\\"red\\"><br /><b>ATTENTION:</b><br /></font><br />\\nThe latest <a href=\\"http://ailla.utexas.org/site/announcements.html\\">News and Announcements</a> about AILLA can be found <a href=\\"http://ailla.utexas.org/site/announcements.html\\">here</a>.\\n</p>\\n\\n<p>AILLA is a digital archive of recordings and texts in and about the indigenous languages of Latin America.</p>\\n<p>\\nAccess to AILLA and its resources is always free of charge. Most of the resources in the collection are available to the public, but some have special <a href=\\"access_restrict.html\\">access restrictions</a>.\\n</p>\\n<p>\\n<b><br />\\nYou must <a href=\\"../user/login.html\\">Register and Login</a> to open or download any media file.<br /></b>\\n</p>\\n<p>\\n<img align=\\"left\\" alt=\\"animal\\" src=\\"/sites/all/themes/multipurpose/images/tiniest_bunny.gif\\" /><b>Users:</b> start by <a href=\\"../search/browse.html?type=menu\\">browsing the catalog</a> or use the navigation menu on the left to find more information about the archive and the indigenous languages of Latin America.\\n</p>\\n<p>\\n<img align=\\"left\\" alt=\\"animal\\" src=\\"/sites/all/themes/multipurpose/images/tiniest_bunny.gif\\" /><b>Depositors:</b> If you want to deposit materials in AILLA, <a href=\\"dep_info.html\\">start here</a>.\\n</p>\\n<p><br /></p><h3>The Collection</h3>\\n<p></p>\\n<p>The heart of the collection is recordings, both audio and video, in a wide range of genres: narratives, chants, oratory, conversations, songs, and more. Many recordings are transcribed and translated into Spanish, English or Portuguese. </p>\\n<p>The archive also contains a wealth of language documentation materials: grammars, dictionaries, ethnographies, and field notes. The collection includes teaching materials for bilingual education and language revitalization programs.</p>\\n<p>\\nWe are committed to serving the indigenous people of Latin America and the scholars who study their languages. We are always interested in hearing from you. Please <a href=\\"mailto:ailla@ailla.utexas.org\\">contact us</a> if you have any questions, comments, or suggestions.\\n</p>\\n\\n<p><img align=\\"left\\" alt=\\"animal\\" src=\\"/sites/all/themes/multipurpose/images/animal.gif\\" /><br /><br /></p>\\n",
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1432818192\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'1\',
      \'comment_count\' => \'0\',
      \'name\' => \'islandora\',
      \'picture\' => \'0\',
      \'data\' => \'b:0;\',
      \'path\' => FALSE,
      \'menu\' => NULL,
      \'node_export_drupal_version\' => \'7\',
    ),
  (object) array(
      \'vid\' => \'8\',
      \'uid\' => \'1\',
      \'title\' => \'Links\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'0\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'7d60d9e9-c3f7-414c-b9ac-5f095f5959f7\',
      \'nid\' => \'8\',
      \'type\' => \'page\',
      \'language\' => \'en\',
      \'created\' => \'1432749294\',
      \'changed\' => \'1432749294\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'7d1a2e9c-34e2-4c57-a7d2-0200b391c55d\',
      \'revision_timestamp\' => \'1432749294\',
      \'revision_uid\' => \'1\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => "dit odio, posuere cursus enim gravida id. Integer ut turpis quis ligula eleifend porttitor. Donec in fringilla metus. Proin bibendum sagittis ligula, eu imperdiet magna molestie eu. Donec eu lectus massa. Pellentesque sit amet tortor mi. Quisque nec elit quis eros fringilla fermentum. Suspendisse porttitor elit quis eros ultricies malesuada. Nunc sollicitudin aliquam mauris, at scelerisque ante suscipit in.\\r\\n\\r\\nPellentesque vestibulum lobortis purus, eget tincidunt massa viverra sit amet. Aenean a magna velit. Cras vulputate dolor leo, et tincidunt nulla ultricies id. Suspendisse euismod, lectus sit amet luctus luctus, velit dolor feugiat lorem, id bibendum neque lorem vel purus. Nulla facilisi. Aenean convallis mauris nec est maximus sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elem",
            \'summary\' => \'\',
            \'format\' => \'filtered_html\',
            \'safe_value\' => "<p>dit odio, posuere cursus enim gravida id. Integer ut turpis quis ligula eleifend porttitor. Donec in fringilla metus. Proin bibendum sagittis ligula, eu imperdiet magna molestie eu. Donec eu lectus massa. Pellentesque sit amet tortor mi. Quisque nec elit quis eros fringilla fermentum. Suspendisse porttitor elit quis eros ultricies malesuada. Nunc sollicitudin aliquam mauris, at scelerisque ante suscipit in.</p>\\n<p>Pellentesque vestibulum lobortis purus, eget tincidunt massa viverra sit amet. Aenean a magna velit. Cras vulputate dolor leo, et tincidunt nulla ultricies id. Suspendisse euismod, lectus sit amet luctus luctus, velit dolor feugiat lorem, id bibendum neque lorem vel purus. Nulla facilisi. Aenean convallis mauris nec est maximus sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elem</p>\\n",
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1432749294\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'1\',
      \'comment_count\' => \'0\',
      \'name\' => \'islandora\',
      \'picture\' => \'0\',
      \'data\' => \'b:0;\',
      \'path\' => FALSE,
      \'menu\' => array(
        \'link_title\' => \'Links\',
        \'mlid\' => 0,
        \'plid\' => \'609\',
        \'menu_name\' => \'main-menu\',
        \'weight\' => \'-47\',
        \'options\' => array(),
        \'module\' => \'menu\',
        \'expanded\' => \'0\',
        \'hidden\' => \'0\',
        \'has_children\' => \'0\',
        \'customized\' => 0,
        \'parent_depth_limit\' => 8,
        \'description\' => \'\',
        \'enabled\' => 1,
      ),
      \'node_export_drupal_version\' => \'7\',
    ),
  (object) array(
      \'vid\' => \'7\',
      \'uid\' => \'1\',
      \'title\' => \'Contact\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'0\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'a82ac6e5-1cb3-4a3e-a24c-6b45cfbd1cfb\',
      \'nid\' => \'7\',
      \'type\' => \'page\',
      \'language\' => \'en\',
      \'created\' => \'1432749267\',
      \'changed\' => \'1432749267\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'80ff2676-1799-4179-9feb-6a08f8ad2147\',
      \'revision_timestamp\' => \'1432749267\',
      \'revision_uid\' => \'1\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => "dit odio, posuere cursus enim gravida id. Integer ut turpis quis ligula eleifend porttitor. Donec in fringilla metus. Proin bibendum sagittis ligula, eu imperdiet magna molestie eu. Donec eu lectus massa. Pellentesque sit amet tortor mi. Quisque nec elit quis eros fringilla fermentum. Suspendisse porttitor elit quis eros ultricies malesuada. Nunc sollicitudin aliquam mauris, at scelerisque ante suscipit in.\\r\\n\\r\\nPellentesque vestibulum lobortis purus, eget tincidunt massa viverra sit amet. Aenean a magna velit. Cras vulputate dolor leo, et tincidunt nulla ultricies id. Suspendisse euismod, lectus sit amet luctus luctus, velit dolor feugiat lorem, id bibendum neque lorem vel purus. Nulla facilisi. Aenean convallis mauris nec est maximus sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elem",
            \'summary\' => \'\',
            \'format\' => \'filtered_html\',
            \'safe_value\' => "<p>dit odio, posuere cursus enim gravida id. Integer ut turpis quis ligula eleifend porttitor. Donec in fringilla metus. Proin bibendum sagittis ligula, eu imperdiet magna molestie eu. Donec eu lectus massa. Pellentesque sit amet tortor mi. Quisque nec elit quis eros fringilla fermentum. Suspendisse porttitor elit quis eros ultricies malesuada. Nunc sollicitudin aliquam mauris, at scelerisque ante suscipit in.</p>\\n<p>Pellentesque vestibulum lobortis purus, eget tincidunt massa viverra sit amet. Aenean a magna velit. Cras vulputate dolor leo, et tincidunt nulla ultricies id. Suspendisse euismod, lectus sit amet luctus luctus, velit dolor feugiat lorem, id bibendum neque lorem vel purus. Nulla facilisi. Aenean convallis mauris nec est maximus sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elem</p>\\n",
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1432749267\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'1\',
      \'comment_count\' => \'0\',
      \'name\' => \'islandora\',
      \'picture\' => \'0\',
      \'data\' => \'b:0;\',
      \'path\' => FALSE,
      \'menu\' => NULL,
      \'node_export_drupal_version\' => \'7\',
    ),
  (object) array(
      \'vid\' => \'5\',
      \'uid\' => \'1\',
      \'title\' => \'Events\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'0\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'f95fc492-07e6-45a7-97c3-c85e34cc53bf\',
      \'nid\' => \'5\',
      \'type\' => \'page\',
      \'language\' => \'en\',
      \'created\' => \'1432749072\',
      \'changed\' => \'1432749072\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'a3e0d15c-0a04-491c-bd43-03c20a51146c\',
      \'revision_timestamp\' => \'1432749072\',
      \'revision_uid\' => \'1\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => "dit odio, posuere cursus enim gravida id. Integer ut turpis quis ligula eleifend porttitor. Donec in fringilla metus. Proin bibendum sagittis ligula, eu imperdiet magna molestie eu. Donec eu lectus massa. Pellentesque sit amet tortor mi. Quisque nec elit quis eros fringilla fermentum. Suspendisse porttitor elit quis eros ultricies malesuada. Nunc sollicitudin aliquam mauris, at scelerisque ante suscipit in.\\r\\n\\r\\nPellentesque vestibulum lobortis purus, eget tincidunt massa viverra sit amet. Aenean a magna velit. Cras vulputate dolor leo, et tincidunt nulla ultricies id. Suspendisse euismod, lectus sit amet luctus luctus, velit dolor feugiat lorem, id bibendum neque lorem vel purus. Nulla facilisi. Aenean convallis mauris nec est maximus sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elem",
            \'summary\' => \'\',
            \'format\' => \'filtered_html\',
            \'safe_value\' => "<p>dit odio, posuere cursus enim gravida id. Integer ut turpis quis ligula eleifend porttitor. Donec in fringilla metus. Proin bibendum sagittis ligula, eu imperdiet magna molestie eu. Donec eu lectus massa. Pellentesque sit amet tortor mi. Quisque nec elit quis eros fringilla fermentum. Suspendisse porttitor elit quis eros ultricies malesuada. Nunc sollicitudin aliquam mauris, at scelerisque ante suscipit in.</p>\\n<p>Pellentesque vestibulum lobortis purus, eget tincidunt massa viverra sit amet. Aenean a magna velit. Cras vulputate dolor leo, et tincidunt nulla ultricies id. Suspendisse euismod, lectus sit amet luctus luctus, velit dolor feugiat lorem, id bibendum neque lorem vel purus. Nulla facilisi. Aenean convallis mauris nec est maximus sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elem</p>\\n",
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1432749072\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'1\',
      \'comment_count\' => \'0\',
      \'name\' => \'islandora\',
      \'picture\' => \'0\',
      \'data\' => \'b:0;\',
      \'path\' => FALSE,
      \'menu\' => NULL,
      \'node_export_drupal_version\' => \'7\',
    ),
  (object) array(
      \'vid\' => \'9\',
      \'uid\' => \'1\',
      \'title\' => \'Contribute\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'0\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'0ae61599-f2af-4435-b3f2-7e2f8011cd3c\',
      \'nid\' => \'9\',
      \'type\' => \'page\',
      \'language\' => \'en\',
      \'created\' => \'1432749331\',
      \'changed\' => \'1432749331\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'c508d6a6-0fec-4c83-897f-ccb0733d2dc7\',
      \'revision_timestamp\' => \'1432749331\',
      \'revision_uid\' => \'1\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => "dit odio, posuere cursus enim gravida id. Integer ut turpis quis ligula eleifend porttitor. Donec in fringilla metus. Proin bibendum sagittis ligula, eu imperdiet magna molestie eu. Donec eu lectus massa. Pellentesque sit amet tortor mi. Quisque nec elit quis eros fringilla fermentum. Suspendisse porttitor elit quis eros ultricies malesuada. Nunc sollicitudin aliquam mauris, at scelerisque ante suscipit in.\\r\\n\\r\\nPellentesque vestibulum lobortis purus, eget tincidunt massa viverra sit amet. Aenean a magna velit. Cras vulputate dolor leo, et tincidunt nulla ultricies id. Suspendisse euismod, lectus sit amet luctus luctus, velit dolor feugiat lorem, id bibendum neque lorem vel purus. Nulla facilisi. Aenean convallis mauris nec est maximus sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elem",
            \'summary\' => \'\',
            \'format\' => \'filtered_html\',
            \'safe_value\' => "<p>dit odio, posuere cursus enim gravida id. Integer ut turpis quis ligula eleifend porttitor. Donec in fringilla metus. Proin bibendum sagittis ligula, eu imperdiet magna molestie eu. Donec eu lectus massa. Pellentesque sit amet tortor mi. Quisque nec elit quis eros fringilla fermentum. Suspendisse porttitor elit quis eros ultricies malesuada. Nunc sollicitudin aliquam mauris, at scelerisque ante suscipit in.</p>\\n<p>Pellentesque vestibulum lobortis purus, eget tincidunt massa viverra sit amet. Aenean a magna velit. Cras vulputate dolor leo, et tincidunt nulla ultricies id. Suspendisse euismod, lectus sit amet luctus luctus, velit dolor feugiat lorem, id bibendum neque lorem vel purus. Nulla facilisi. Aenean convallis mauris nec est maximus sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elem</p>\\n",
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1432749331\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'1\',
      \'comment_count\' => \'0\',
      \'name\' => \'islandora\',
      \'picture\' => \'0\',
      \'data\' => \'b:0;\',
      \'path\' => FALSE,
      \'menu\' => array(
        \'link_title\' => \'Contribute\',
        \'mlid\' => 0,
        \'plid\' => \'609\',
        \'menu_name\' => \'main-menu\',
        \'weight\' => \'-46\',
        \'options\' => array(),
        \'module\' => \'menu\',
        \'expanded\' => \'0\',
        \'hidden\' => \'0\',
        \'has_children\' => \'0\',
        \'customized\' => 0,
        \'parent_depth_limit\' => 8,
        \'description\' => \'\',
        \'enabled\' => 1,
      ),
      \'node_export_drupal_version\' => \'7\',
    ),
  (object) array(
      \'vid\' => \'3\',
      \'uid\' => \'1\',
      \'title\' => \'AILLA Papers\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'0\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'814e9827-dd3c-4d01-8440-5516807d9f3c\',
      \'nid\' => \'3\',
      \'type\' => \'page\',
      \'language\' => \'en\',
      \'created\' => \'1432748939\',
      \'changed\' => \'1432748939\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'d3dead96-5202-4497-8ea4-d5934d16cae0\',
      \'revision_timestamp\' => \'1432748939\',
      \'revision_uid\' => \'1\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => "Pellentesque vestibulum lobortis purus, eget tincidunt massa viverra sit amet. Aenean a magna velit. Cras vulputate dolor leo, et tincidunt nulla ultricies id. Suspendisse euismod, lectus sit amet luctus luctus, velit dolor feugiat lorem, id bibendum neque lorem vel purus. Nulla facilisi. Aenean convallis mauris nec est maximus sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elementum velit id mattis luctus. Duis at justo consequat, porttitor nibh vitae, vehicula massa. Aliquam quis arcu iaculis, accumsan ex ut, blandit odio. Suspendisse urna sapien, sodales sed enim malesuada, aliquam lobortis lorem.\\r\\n\\r\\nSed felis quam, pretium a ipsum a, pretium laoreet turpis. Donec ex eros, vulputate nec erat in, sollicitudin varius eros. Sed id lorem lobortis, laoreet tortor fermentum, consequat ligula. Sed sagittis ullamcorper ligula. Aenean non nisi auctor, tincidunt erat sit amet, pharetra est. Cras mi tortor, vestibulum vel cursus finibus, ullamcorper eget diam. Quisque et ornare odio. Vivamus quis egestas mi. Duis nec odio vel libero hendrerit dapibus. Nullam mollis egestas cursus. Donec feugiat convallis fermentum. Quisque efficitur dignissim velit et dignissim. Nullam tempor ex ac accumsan semper. Maecenas ut finibus sem, nec scelerisque risus. Nulla feugiat blandit consectetur.\\r\\n\\r\\nMaecenas bibendum placerat turpis, ultricies commodo tellus dictum in. Aliquam ante nibh, vestibulum et lacinia nec, congue eget lorem. Proin malesuada erat vel nisi tempor, sollicitudin rutrum turpis tincidunt. Donec eleifend ac nulla vel convallis. Aenean ac elit turpis. Interdum et malesuada fames ac ante ipsum primis in faucibus. Maecenas in interdum dolor. Suspendisse tincidunt imperdiet tortor sed laoreet. Duis viverra bibendum consequat. Phasellus ultricies facilisis nunc, a molestie odio facilisis eu. Duis eu dui in tortor consequat elementum. Aliquam at lacus at urna rutrum auctor. Quisque ut sodales risus. ",
            \'summary\' => \'\',
            \'format\' => \'filtered_html\',
            \'safe_value\' => "<p>Pellentesque vestibulum lobortis purus, eget tincidunt massa viverra sit amet. Aenean a magna velit. Cras vulputate dolor leo, et tincidunt nulla ultricies id. Suspendisse euismod, lectus sit amet luctus luctus, velit dolor feugiat lorem, id bibendum neque lorem vel purus. Nulla facilisi. Aenean convallis mauris nec est maximus sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elementum velit id mattis luctus. Duis at justo consequat, porttitor nibh vitae, vehicula massa. Aliquam quis arcu iaculis, accumsan ex ut, blandit odio. Suspendisse urna sapien, sodales sed enim malesuada, aliquam lobortis lorem.</p>\\n<p>Sed felis quam, pretium a ipsum a, pretium laoreet turpis. Donec ex eros, vulputate nec erat in, sollicitudin varius eros. Sed id lorem lobortis, laoreet tortor fermentum, consequat ligula. Sed sagittis ullamcorper ligula. Aenean non nisi auctor, tincidunt erat sit amet, pharetra est. Cras mi tortor, vestibulum vel cursus finibus, ullamcorper eget diam. Quisque et ornare odio. Vivamus quis egestas mi. Duis nec odio vel libero hendrerit dapibus. Nullam mollis egestas cursus. Donec feugiat convallis fermentum. Quisque efficitur dignissim velit et dignissim. Nullam tempor ex ac accumsan semper. Maecenas ut finibus sem, nec scelerisque risus. Nulla feugiat blandit consectetur.</p>\\n<p>Maecenas bibendum placerat turpis, ultricies commodo tellus dictum in. Aliquam ante nibh, vestibulum et lacinia nec, congue eget lorem. Proin malesuada erat vel nisi tempor, sollicitudin rutrum turpis tincidunt. Donec eleifend ac nulla vel convallis. Aenean ac elit turpis. Interdum et malesuada fames ac ante ipsum primis in faucibus. Maecenas in interdum dolor. Suspendisse tincidunt imperdiet tortor sed laoreet. Duis viverra bibendum consequat. Phasellus ultricies facilisis nunc, a molestie odio facilisis eu. Duis eu dui in tortor consequat elementum. Aliquam at lacus at urna rutrum auctor. Quisque ut sodales risus.</p>\\n",
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1432748939\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'1\',
      \'comment_count\' => \'0\',
      \'name\' => \'islandora\',
      \'picture\' => \'0\',
      \'data\' => \'b:0;\',
      \'path\' => FALSE,
      \'menu\' => NULL,
      \'node_export_drupal_version\' => \'7\',
    ),
  (object) array(
      \'vid\' => \'4\',
      \'uid\' => \'1\',
      \'title\' => \'Depositors\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'0\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'978e9180-fb70-40c0-823a-212589e9eb2d\',
      \'nid\' => \'4\',
      \'type\' => \'page\',
      \'language\' => \'en\',
      \'created\' => \'1432749003\',
      \'changed\' => \'1432749003\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'f0642b9b-25f1-4214-8b3a-b1f89fd273e2\',
      \'revision_timestamp\' => \'1432749003\',
      \'revision_uid\' => \'1\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => " Sed felis quam, pretium a ipsum a, pretium laoreet turpis. Donec ex eros, vulputate nec erat in, sollicitudin varius eros. Sed id lorem lobortis, laoreet tortor fermentum, consequat ligula. Sed sagittis ullamcorper ligula. Aenean non nisi auctor, tincidunt erat sit amet, pharetra est. Cras mi tortor, vestibulum vel cursus finibus, ullamcorper eget diam. Quisque et ornare odio. Vivamus quis egestas mi. Duis nec odio vel libero hendrerit dapibus. Nullam mollis egestas cursus. Donec feugiat convallis fermentum. Quisque efficitur dignissim velit et dignissim. Nullam tempor ex ac accumsan semper. Maecenas ut finibus sem, nec scelerisque risus. Nulla feugiat blandit consectetur.\\r\\n\\r\\nMaecenas bibendum placerat turpis, ultricies commodo tellus dictum in. Aliquam ante nibh, vestibulum et lacinia nec, congue eget lorem. Proin malesuada erat vel nisi tempor, sollicitudin rutrum turpis tincidunt. Donec eleifend ac nulla vel convallis. Aenean ac elit turpis. Interdum et malesuada fames ac ante ipsum primis in faucibus. Maecenas in interdum dolor. Suspendisse tincidunt imperdiet tortor sed laoreet. Duis viverra bibendum consequat. Phasellus ultricies facilisis nunc, a molestie odio facilisis eu. Duis eu dui in tortor consequat elementum. Aliquam at lacus at urna rutrum auctor. Quisque ut sodales risus. ",
            \'summary\' => \'\',
            \'format\' => \'filtered_html\',
            \'safe_value\' => "<p>Sed felis quam, pretium a ipsum a, pretium laoreet turpis. Donec ex eros, vulputate nec erat in, sollicitudin varius eros. Sed id lorem lobortis, laoreet tortor fermentum, consequat ligula. Sed sagittis ullamcorper ligula. Aenean non nisi auctor, tincidunt erat sit amet, pharetra est. Cras mi tortor, vestibulum vel cursus finibus, ullamcorper eget diam. Quisque et ornare odio. Vivamus quis egestas mi. Duis nec odio vel libero hendrerit dapibus. Nullam mollis egestas cursus. Donec feugiat convallis fermentum. Quisque efficitur dignissim velit et dignissim. Nullam tempor ex ac accumsan semper. Maecenas ut finibus sem, nec scelerisque risus. Nulla feugiat blandit consectetur.</p>\\n<p>Maecenas bibendum placerat turpis, ultricies commodo tellus dictum in. Aliquam ante nibh, vestibulum et lacinia nec, congue eget lorem. Proin malesuada erat vel nisi tempor, sollicitudin rutrum turpis tincidunt. Donec eleifend ac nulla vel convallis. Aenean ac elit turpis. Interdum et malesuada fames ac ante ipsum primis in faucibus. Maecenas in interdum dolor. Suspendisse tincidunt imperdiet tortor sed laoreet. Duis viverra bibendum consequat. Phasellus ultricies facilisis nunc, a molestie odio facilisis eu. Duis eu dui in tortor consequat elementum. Aliquam at lacus at urna rutrum auctor. Quisque ut sodales risus.</p>\\n",
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1432749003\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'1\',
      \'comment_count\' => \'0\',
      \'name\' => \'islandora\',
      \'picture\' => \'0\',
      \'data\' => \'b:0;\',
      \'path\' => FALSE,
      \'menu\' => NULL,
      \'node_export_drupal_version\' => \'7\',
    ),
)',
);
  return $node_export;
}
