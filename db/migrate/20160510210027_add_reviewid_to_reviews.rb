class AddReviewidToReviews < ActiveRecord::Migration
  def change
    add_column :reviews, :review_id, :integer
  end
end
