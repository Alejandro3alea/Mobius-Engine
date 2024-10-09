#pragma once

#define unique_vector(class_name) std::vector< std::unique_ptr<class_name> >
#define shared_vector(class_name) std::vector< std::shared_ptr<class_name> >