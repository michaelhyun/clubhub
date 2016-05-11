class Club < ActiveRecord::Base
	belongs_to :category
	has_many :reviews
	ratyrate_rateable "rating"
end
