class Club < ActiveRecord::Base
	belongs_to :category
	has_many :reviews
<<<<<<< HEAD

=======
	ratyrate_rateable "rating"
>>>>>>> origin/ka-branch
end
