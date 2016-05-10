class AddClubidToReviews < ActiveRecord::Migration
  def change
    add_column :reviews, :club_id, :integer
  end
end
